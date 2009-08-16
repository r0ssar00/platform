/**
 * @file database.cpp
 * @license GPLv2
 * @author Kevin Ross <r0ssar00@gmail.com> Copyright (C) 2009 Kevin Ross
 */
#include <iostream>
#include <stdio.h>
#include <platform/sqlite3x.hpp>
#include <cc++/file.h>
#include <platform/database.h>
using namespace std;
str spacer(str data, int width) {
	if (data.length() > width) {
		return data.substr(0,width-data.length());
	} else if (data.length() == width) {
		return data;
	}
	string ret = data;
	for (int i = data.length(); i < width; i++) {
		ret+=" ";
	}
	return ret;
}
vector<string> split(const string& s, const string& delim, const bool keep_empty = true) {
    vector<string> result;
    if (delim.empty()) {
        result.push_back(s);
        return result;
    }
    string::const_iterator substart = s.begin(), subend;
    while (true) {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        string temp(substart, subend);
        if (keep_empty || !temp.empty()) {
            result.push_back(temp);
        }
        if (subend == s.end()) {
            break;
        }
        substart = subend + delim.size();
    }
    return result;
}


str int2str(int a) {
	str o = "";
	char b[ 4 ];
	sprintf(b, "%d", a);
	o += b;
	return o;
};
str type_to_string(db_types_t data) {
	if (data == db_type_int) {
		return str("INTEGER");
	} else if (data==db_type_str) {
		return str("TEXT");
	} else if (data==db_type_intkey) {
		return str("INTEGER PRIMARY KEY");
	}
	return "NULL";
}
db_types_t string_to_type(str data) {
	if (data=="INTEGER") {
		return db_type_int;
	} else if (data=="TEXT") {
		return db_type_str;
	} else if (data=="INTEGER PRIMARY KEY") {
		return db_type_intkey;
	} else {
		return db_type_null;
	}
}
Object::Object() {
	d_type = db_type_null;
}
Object::Object(str str_i) {
	d_type = db_type_str;
	data_str = str_i;
}
Object::Object(int int_i) {
	d_type = db_type_int;
	data_int = int_i;
}
db_types_t Object::get_type() const {
	return d_type;
}
bool Object::is_int() const {
	return d_type == db_type_int;
}
bool Object::is_string() const {
	return d_type == db_type_str;
}
bool Object::is_null() const {
	return d_type == db_type_null;
}
bool Object::compare (Object & a) {
	if (this->is_int()) {
		if (a.is_int()) {
			return this->as_int()<a.as_int();
		} else {
			return this->as_string()<a.as_string();
		}
	} else {
		return this->as_string()<a.as_string();
	}
}
str Object::as_string() const {
	if (d_type == db_type_int) {
		return int2str(data_int);
	}
	return data_str;
}
int Object::as_int() const {
	return data_int;
}
Object Object::operator ()() {
	return *this;
}
Row::Row() {
}
Row::Row(col_list cols) {
	data_columns = cols;
}
Row::Row(col_obj_map data) {
	data_set = data;
}
Row * Row::add(Column col, Object data) {
	data_set.insert(col_obj_pair(col,data));
	return this;
}
Row * Row::add(str col, Object data) {
	return this;
}
size_t Row::column_count() {
	return data_set.size();
}
Object Row::data_for_col(Column col) {
	col_obj_map::iterator i = data_set.find(col);
	if (i!=data_set.end()) {
		return i->second();
	}
	return Object();
}
str Row::to_string() {
	str ret = "";/*
	col_obj_map::iterator i = data_set.begin();
	for (; i < data_set.end(); ++i) {
		// i->first = col, i->second = data
		ret+=spacer(i->second().as_string(), 10);
	}*/
	return ret;
}
Column::Column(str sql_string, int index) {
	// data looks like "name TYPE"
	data_sql_string = sql_string;
	str_list tokens;
	tokens = split(sql_string, " ");
	data_name = tokens[0];
	str type = "";
	for (str_list_i i = tokens.begin(); i != tokens.end(); ++i) {
		type += *i;
	}
	data_type = string_to_type(type);
	data_index = index;
}
Column::Column(str name, db_types_t type) {
	data_name = name;
	data_type = type;
	data_index = - 1;
}
Column::Column(str name, db_types_t type, int index) {
	data_name = name;
	data_type = type;
	data_index = index;
}
str Column::get_name() const {
	return data_name;
}
db_types_t Column::get_type() const {
	return data_type;
}
int Column::get_index() const {
	return data_index;
}

bool operator <(const Column&a, const Column& b) {
	return a.get_name()<b.get_name();
}
Table::Table(Database *db, str name, bool exists) {
	data_db = db;
	data_name = name;
	num_columns = 0;
	data_columns = new col_list();
	if (exists) return;
	error = data_db->execute_statement("CREATE TABLE IF NOT EXISTS " + name + " (row INTEGER PRIMARY KEY)", defer_no);
}
Table::Table(Database *db, str name, bool exists, ...) {
	data_db = db;
	data_name = name;
	num_columns = 0;
	data_columns = new col_list();
	if (exists) return;
	va_list args;
	va_start(args, exists);
	while (data_columns->back().get_type()!=db_type_null) {
		data_columns->push_back(Column(va_arg(args, const char *), num_columns++));
	}
	va_end(args);
}
Table::Table(Database *db, str name, col_list &columns, bool exists) {
	Table_init(db, name, columns, exists);
}
void Table::Table_init(Database *db, str name, col_list &columns, bool exists) {
	data_db = db;
	data_name = name;
	num_columns = 0;
	data_columns = new col_list();
	// primary key
	data_columns->push_back(Column("row", db_type_intkey, num_columns++));
	// rest of the columns
	for (col_list_i i = columns.begin(); i != columns.end(); ++i) {
		data_columns->push_back(Column(i->get_name(), i->get_type(), num_columns++));
	}
	if (exists) return;
	str query = "CREATE TABLE IF NOT EXISTS " + name + " (";
	for (col_list_i i = data_columns->begin(); i != data_columns->end(); ++i) {
		query += i->get_name() + " ";
		query+=type_to_string(i->get_type());
		query += ", ";
	}
	query.erase(query.length() - 2, 2);
	query += ")";
	error = data_db->execute_statement(query, defer_no);
}
Table::~Table() {
	delete data_columns;
}
class generate_add {
public:
	generate_add(str & query, Row & data) {
		data_query = query;
		row_data = data;
	}
	void operator() (const Column & data) {
		if (!row_data.data_for_col(data).is_null()) {
			data_query += prefix(data);
			data_query += row_data.data_for_col(data).is_string() ? "\"" : "";
			data_query += row_data.data_for_col(data).as_string();
			data_query += row_data.data_for_col(data).is_string() ? "\"" : "";
			data_query += ", ";
		}
	}
private:
	str data_query;
	Row row_data;
	str prefix(const Column & data) {
		return "";
	}
};
class generate_mod : public generate_add {
public:
	generate_mod(str & query, Row & data) : generate_add(query, data) {};
private:
	str prefix(const Column & data) {
		return data.get_name() + " = ";
	}
};
void Table::add_row(Row &data, defer_types_t deferred) {
	if (data.column_count()+1 != data_columns->size()) {
		error = 1;
		return;
	}
	str query = "INSERT INTO " + data_name + " VALUES (NULL, ";
	col_list_i i = data_columns->begin();
	i++;
	/*
	for (; i != data_columns->end(); ++i) {
		query += data.data_for_col(*i).is_string() ? "\"" : "";
		query += data.data_for_col(*i).as_string();
		query += data.data_for_col(*i).is_string() ? "\"" : "";
		query += ", ";
	}
	 */
	std::for_each(i, data_columns->end(), generate_add(query, data));
	query.erase(query.length() - 2, 2);
	query += ")";
	error = data_db->execute_statement(query, deferred);
}

void Table::mod_row(int rowid, Row &data, defer_types_t deferred) {
// UPDATE OR IGNORE table_name SET column1 = value1, column2 = value2, ... WHERE row = rowid
	str query = "UPDATE OR IGNORE " + data_name + " SET ";
	col_list_i j = data_columns->begin();
	j++;
	/*
	for (; j != data_columns->end(); ++j) {
		query += j->get_name();
		query += " = ";
		query += data.data_for_col(*j).is_string()?"\"":"";
		query += data.data_for_col(*j).as_string();
		query += data.data_for_col(*j).is_string()?"\"":"";
		query += ", ";
	}
	 */
	std::for_each(j, data_columns->end(), generate_mod(query, data));
	query.erase(query.length() - 2, 2);
	query += " WHERE row = " + int2str(rowid);
	error = data_db->execute_statement(query, deferred);
}

void Table::add_column(Column column, bool exists) {
	if (!exists) {
		str query = "ALTER TABLE " + data_name + " ADD COLUMN ";
		query += column.get_name() + " ";
		query += column.get_type() == db_type_int ? "INTEGER" : column.get_type() == db_type_str ? "TEXT" : "";
		error = data_db->execute_statement(query, defer_no);
	}
	if (!error || exists)
		data_columns->push_back(Column(column.get_name(), column.get_type(), num_columns++));
}
void Table::add_columns(col_list &columns, bool exists) {
	for (col_list_i i = columns.begin(); i != columns.end(); ++i) {
		add_column(*i, exists);
	}
}
void Table::add_columns(bool exists, ...) {
	va_list args;
	va_start(args, exists);
	while (data_columns->back().get_type()!=db_type_null) {
		data_columns->push_back(Column(va_arg(args, const char *), num_columns++));
	}
	va_end(args);
}
row_list Table::query(str query) {
	row_list results;
	error = data_db->execute_query(query, *this, results);
	return results;
}
str Table::get_name() {
	return data_name;
}
col_list *Table::get_columns() {
	return data_columns;
}
Column Table::get_column(str name) {
	for (col_list_i i = data_columns->begin(); i!= data_columns->end(); ++i) {
		if (i->get_name()==name) {
			return *i;
		}
	}
	return Column("NULL", db_type_null);
}
row_list Table::get_rows() {
	return query("*");
}
int Table::get_error() {
	return error;
}
Database::Database() {
	loaded = false;
}
Database::Database(str file) {
	ost::ThreadFile *dbfile = new ost::ThreadFile(file.c_str());
	ost::File::Error err = dbfile->getErrorNumber();
	if (err == ost::File::errOpenFailed) {
		loaded = false;
		delete dbfile;
		return;
	}
	try {
		DB.open(file);
		loaded = true;
	} catch(sqlite3x::database_error e) {
		loaded = false;
		delete dbfile;
		return;
	}
	delete dbfile;
}
int Database::execute_query(str selector, Table &table, row_list &results) {
	str query = "SELECT " + selector + " FROM " + table.get_name();
	col_list columns = *table.get_columns();
	col_obj_map tempdata;
	try {
		sqlite3x::sqlite3_command cmd(DB, query);
		sqlite3x::sqlite3_cursor cur(cmd.executecursor());
		while (cur.step()) {
			for (col_list_i i = columns.begin(); i != columns.end(); ++i) {
				if (i->get_type() == db_type_int) {
					tempdata.insert(col_obj_pair(*i,Object(cur.getint(i->get_index()))));
				} else if (i->get_type() == db_type_str) {
					tempdata.insert(col_obj_pair(*i,Object(cur.getstring(i->get_index()))));
				}
			}
			results.push_back(Row(tempdata));
		}
	} catch(sqlite3x::database_error e) {
		cout << e.what() << endl;
		return DB.errorcode();
	}
	return DB.errorcode();
}
int Database::execute_statement(str statement, defer_types_t deferred) {
	if (deferred == defer_yes) {
		deferred_statements.push_back(statement);
		return 0;
	}
	try {
		sqlite3x::sqlite3_command cmd(DB, statement);
		cmd.executenonquery();
	} catch(sqlite3x::database_error e) {
		cout << e.what() << endl;
		return DB.errorcode();
	}
	return DB.errorcode();
}
int Database::commit() {
	try {
		sqlite3x::sqlite3_transaction trans(DB);
		{
			for (str_list_i i = deferred_statements.begin(); i != deferred_statements.end(); ++i) {
				sqlite3x::sqlite3_command cmd(DB, i->c_str());
				cmd.executenonquery();
			}
		}
		trans.commit();
	} catch(sqlite3x::database_error e) {
		cout << e.what() << endl;
		return DB.errorcode();
	}
	return DB.errorcode();
}

void Database::rollback() {
	deferred_statements.clear();
}

bool Database::is_loaded() {
	return loaded;
}
