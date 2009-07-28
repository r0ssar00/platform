/**
 * @file database.cpp
 * @license GPLv2
 * @author Kevin Ross <r0ssar00@gmail.com> Copyright (C) 2009 Kevin Ross
 */
#include <list>
#include <iostream>
#include <string>
#include <stdio.h>
#include "sqlite3x.hpp"
#include <cc++/file.h>
#include "database.h"
void Tokenize(const std::string& str, std::list<std::string>& tokens, const std::string& delimiters = " ") {
    // Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
	std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
	
    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

std::string int2str(int a) {
	std::string o = "";
	char b[ 4 ];
	sprintf(b, "%d", a);
	o += b;
	return o;
};
Object::Object(std::string str) {
	d_type = db_type_str;
	data_str = str;
}
Object::Object(int integer) {
	d_type = db_type_int;
	data_int = integer;
}
db_types_t Object::get_type() {
	return d_type;
}
bool Object::is_int() {
	return d_type == db_type_int;
}
bool Object::is_string() {
	return d_type == db_type_str;
}
std::string Object::as_string() {
	if (d_type == db_type_int) {
		return int2str(data_int);
	}
	return data_str;
}
int Object::as_int() {
	return data_int;
}
Row::Row() {
}
Row::Row(std::list<Object> &data) {
	dataset = data;
}
std::list<Object>::iterator Row::begin() {
	return dataset.begin();
}
std::list<Object>::iterator Row::end() {
	return dataset.end();
}
int Row::column_count() {
	return dataset.size();
}
Column::Column(std::string name, db_types_t type) {
	data_name = name;
	data_type = type;
	data_index = -1;
}
Column::Column(std::string name, db_types_t type, int index) {
	data_name = name;
	data_type = type;
	data_index = index;
}
std::string Column::get_name() {
	return data_name;
}
db_types_t Column::get_type() {
	return data_type;
}
int Column::get_index() {
	return data_index;
}
Table::Table(Database *db, std::string name, bool exists) {
	data_db = db;
	data_name = name;
	num_columns = 1;
	data_columns = new std::list<Column>();
	if (exists) return;
	error = data_db->execute_statement("CREATE TABLE IF NOT EXISTS " + name + " (row INTEGER PRIMARY KEY)", defer_none);
}
Table::Table(Database *db, std::string name, std::list<Column> &columns, bool exists) {
	Table_init(db,name,columns,exists);
}
Table::Table(Database * db, std::string name, std::string columns, bool exists) {
	//columns looks like: "name type, name type, name type"
	std::list<std::string> parts,colparts;
	std::list<Column> cols;
	std::list<std::string>::iterator j;
	std::string colname;
	db_types_t coltype;
	Tokenize(columns,parts,", ");
	for (std::list<std::string>::iterator i=parts.begin(); i!=parts.end(); ++i) {
		Tokenize(*i,colparts);
		j = colparts.begin();
		colname = *(j++);
		coltype = *j=="INTEGER"?db_type_int:db_type_str;
		cols.push_back(Column(colname,coltype));
		j = NULL;
		colparts.clear();
	}
	Table_init(db, name, cols, exists);
}
void Table::Table_init(Database *db, std::string name, std::list<Column> &columns, bool exists) {
	data_db = db;
	data_name = name;
	num_columns = 1;
	data_columns = new std::list<Column>();
	for (std::list<Column>::iterator i = columns.begin(); i!= columns.end(); ++i) {
		data_columns->push_back(Column(i->get_name(), i->get_type(), num_columns++));
	}
	if (exists) return;
	std::string query = "CREATE TABLE IF NOT EXISTS " + name + " (row INTEGER PRIMARY KEY, ";
	for (std::list<Column>::iterator i = columns.begin(); i != columns.end(); ++i) {
		query += i->get_name() + " ";
		if (i->get_type() == db_type_int) {
			query += "INTEGER";
		} else if (i->get_type() == db_type_str) {
			query += "TEXT";
		}
		query += ", ";
	}
	query.erase(query.length() - 2, 2);
	query += ")";
	error = data_db->execute_statement(query,defer_none);
}
Table::~Table() {
	delete data_columns;
}
void Table::add_row(Row &data, defer_types_t deferred) {
	if (data.column_count()!=data_columns->size()) {
		error = 1;
		return;
	}
	std::string query = "INSERT INTO " + data_name + " VALUES (NULL, ";
	for (std::list<Object>::iterator i = data.begin(); i != data.end(); ++i) {
		query += i->is_string() ? "\"" : "";
		query += i->is_string() ? i->as_string() : int2str(i->as_int());
		query += i->is_string() ? "\"" : "";
		query += ", ";
	}
	query.erase(query.length() - 2, 2);
	query += ")";
	error = data_db->execute_statement(query, deferred);
}
void Table::add_column(Column column, bool exists) {
	if (!exists) {
		std::string query = "ALTER TABLE " + data_name + " ADD COLUMN ";
		query += column.get_name() + " ";
		query += column.get_type() == db_type_int ? "INTEGER" : "TEXT";
		error = data_db->execute_statement(query, defer_none);
	}
	if (!error || exists)
		data_columns->push_back(Column(column.get_name(),column.get_type(),num_columns++));
}
void Table::add_columns(std::list<Column> & columns, bool exists) {
	for (std::list<Column>::iterator i=columns.begin(); i!=columns.end(); ++i) {
		add_column(*i, exists);
	}
}
std::list<Row> Table::query(std::string query) {
	std::list<Row> results;
	std::list<Row> *workresults = new std::list<Row>();
	error = data_db->execute_query(query, *this, workresults);
	if (!error)
		results = *workresults;
	delete workresults;
	return results;
}
std::string Table::get_name() {
	return data_name;
}
std::list<Column> *Table::get_columns() {
	return data_columns;
}
std::list<Row> Table::get_rows() {
	return query("*");
}
int Table::get_error() {
	return error;
}
Database::Database() {
	loaded = false;
}
Database::Database(std::string file) {
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
int Database::execute_query(std::string selector, Table &table, std::list<Row> * &results) {
	lock();
	std::string query = "SELECT " + selector + " FROM " + table.get_name();
	std::list<Column> columns = *table.get_columns();
	std::list<Object> tempdata;
	try {
		sqlite3x::sqlite3_command cmd(DB, query);
		sqlite3x::sqlite3_cursor cur(cmd.executecursor());
		while (cur.step()) {
			for (std::list<Column>::iterator i = columns.begin(); i != columns.end(); ++i) {
				if (i->get_type() == db_type_int) {
					tempdata.push_back(Object(cur.getint(i->get_index())));
				} else if (i->get_type() == db_type_str) {
					tempdata.push_back(Object(cur.getstring(i->get_index())));
				}
				results->push_back(Row(tempdata));
			}
		}
	} catch(sqlite3x::database_error e) {
		std::cout << e.what() << std::endl;
		unlock();
		return DB.errorcode();
	}
	unlock();
	return DB.errorcode();
}
int Database::execute_statement(std::string statement, defer_types_t deferred) {
	lock();
	if (deferred==defer_force) {
		deferred_statements.push_back(statement);
		return 0;
	}
	try {
		sqlite3x::sqlite3_command cmd(DB, statement);
		cmd.executenonquery();
	} catch(sqlite3x::database_error e) {
		std::cout << e.what() << std::endl;
		unlock();
		return DB.errorcode();
	}
	unlock();
	return DB.errorcode();
}
int Database::commit() {
	lock();
	try {
		sqlite3x::sqlite3_transaction trans(DB);
		{
			for (std::list<std::string>::iterator i=deferred_statements.begin(); i!=deferred_statements.end(); ++i) {
				sqlite3x::sqlite3_command cmd(DB,i->c_str());
				cmd.executenonquery();
			}
		}
		trans.commit();
	} catch(sqlite3x::database_error e) {
		std::cout << e.what() << std::endl;
		unlock();
		return DB.errorcode();
	}
	unlock();
	return DB.errorcode();
}

void Database::rollback() {
	deferred_statements.clear();
}

bool Database::is_loaded() {
	return loaded;
}

void Database::lock() {
	// block until the lock is released
	while (locked) {
		timespec t;
		t.tv_nsec=100;
		nanosleep(&t, (timespec*)NULL);
	}
	locked = true;
}
void Database::unlock() {
	locked = false;
}