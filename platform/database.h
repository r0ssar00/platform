#ifndef PLATFORM_DATABASE_H
#define PLATFORM_DATABASE_H
#include <map>
#include <vector>
#include <string>
#include <platform/sqlite3x.hpp>
/** @file database.h
 *  @license GPLv2
 *  @author Kevin Ross <r0ssar00@gmail.com> Copyright (C) 2009 Kevin Ross
 *  @short Encapsulates database access in easy to use objects
 *
 *  @details Allows really easy access to an sqlite database without having to worry
 *  about sqlite statements.  Core class is the Database.  You create Tables associated
 *  with a database to which you can add Columns and then Rows.
 */
/**
 * @short the column type enumeration
 */
enum db_types_t {
	//! integer type
	db_type_int = 0,
	//! string type
	db_type_str,
	//! primary key type
	db_type_intkey,
	//! null type
	db_type_null
};

/**
 * @short the defer type enumeration
 */
enum defer_types_t {
	//! Force the call to defer until commit is called
	defer_force = 0,
	//! Force the call to commit immediately
	defer_none
};

class Database;
class Table;
class Column;
class Row;
class Object;

/**
 * @short the typedefs
 */
typedef std::string str;
typedef std::vector<Column> col_list;
typedef col_list::iterator col_list_i;
typedef std::vector<Row> row_list;
typedef row_list::iterator row_list_i;
typedef std::vector<str> str_list;
typedef str_list::iterator str_list_i;
typedef std::vector<Object> obj_list;
typedef obj_list::iterator obj_list_i;
typedef std::map<Column, Object> col_obj_map;
typedef std::pair<Column, Object> col_obj_pair;

// type converters
str type_to_string(db_types_t data);
db_types_t string_to_type(str data);

/** @short The core data storage object
 *  @details The Object stores several datatypes(described in db_types_t).  You will use
 *  this class heavily whenever developing a program using the database.
 */

class Object {
public:
	//! @short Creates a useless Object
	//! @details Don't use the resulting Object because it contains no value
	//! and will spew useless garbage.
	Object();
	//! @short Creates a string Object
	//! @param str_i The string to initialize the Object with
	Object(str str_i);
	//! @short Creates an int Object
	//! @param int_i The int to init the Object with
	Object(int int_i);
	/** @short gets the type of the Object
	 *  @return the type
	 */
	db_types_t get_type() const;
	/** @short gets whether the Object is an int
	 *  @return true if int, false otherwise
	 */
	bool is_int() const;
	/** @short gets whether the Object is a string
	 *  @return true if string, false otherwise
	 */
	bool is_string() const;
	//! @short gets whether the Object is null
	//! @return true if null, false otherwise
	bool is_null() const;
	/** @short returns the internal data as a string.
	 *  @details if the Object represents an int, this
	 *  will return the string representation of the int, eg int(5)=string("5")
	 *  @return string value of internal data
	 */
	str as_string() const;
	/** @short returns the internal data as int.
	 *  @details this will NOT convert the string
	 *  into any form.  it will fail.
	 *  @return int value of internal data
	 */
	int as_int() const;
	
	bool compare (Object & a);
	
	Object operator ()();
private:
	str data_str; // stores the string data
	int data_int; // stores the int data
	db_types_t d_type; // stores the type
};
/** @short Row stores the data for an entire row.
 *  @details Row stores this data in a list which you can iterate through using
 *  the begin() and end() methods provided.  begin() is the first column and end()
 *  the last from left to right.
 */
class Row {
public:
	//! Creates an empty Row
	Row();
	//! @short Creates a Row, filling it with data
	//! @param data The initial data to fill the Row with
	Row(col_obj_map data);
	//! @short Add data using a chain
	//! @details Use this method like: <code>instance->add(Column,Object)->add(Column,Object)...</code>
	//! @param col The Column the data fits in
	//! @param data The column data to add
	Row * add(Column col, Object data);
	//! @short Get the row data as a Column-Object mapping of data
	//! @param The columns defined for this row
	col_obj_map as_col_obj_map();
	//! @short Get the data for a given Column
	//! @param col The column to get the data for
	//! @return the data
	Object data_for_col(Column col);
	//! @short Get the number of columns in this row
	//! @return The number of columns
	size_t column_count();
private:
	col_obj_map data_set; // stores the column data
};
/**
 * @short The data for a column.
 * @details This class stores the data for a column.  Developers
 * creating a table should use Column(string, db_types_t) because
 * it will automatically track the column index for you.  If you(the developer)
 * are performing a query and want only a specific set of columns,
 * use Column(string, db_types_t, int) where the last param is the index
 * of the column you want.
 */
class Column {
public:
	//! @short Creates a new column with a name and type combined in a string
	//! @details Use strings like "name TYPE" where TYPE is TEXT,INTEGER.  The string
	//! should be a valid SQL type
	//! @param sql_string SQL type
	Column(str sql_string, int index = -1);
	//! @short Creates a new column with a name and type
	//! @param name The name of the column
	//! @param type The type of the column
	Column(str name, db_types_t type);
	//! @short Creates a new column with a name, type, and index.
	//! @details Using this constructor means you have to
	//! keep track of the number of columns manually.
	//! Don't use it because it's a whole lot easier to
	//! use Column(str, db_types_t).
	//! @param name The name of the column
	//! @param type The type of the column
	//! @param index The index of the column
	Column(str name, db_types_t type, int index = -1);
	//! @short Get the data type of the Column
	//! @return the type
	db_types_t get_type() const;
	//! @short Get the name of the Column
	//! @return the name
	str get_name() const;
	//! @short Get the index of the Column
	//! @details This is used to get this column's index.
	//! You won't need to use it.  It's here for the Table
	//! and Database classes to use
	//! @return the column's index
	int get_index() const;
	str get_sql_string() const;
	friend bool operator <(const Column&a, const Column&b);
private:
	str data_name; // stores the column name
	db_types_t data_type;  // stores the column tyoe
	int data_index;        // stores the column index
	str data_sql_string; // stores the sql string
};
/** @short Stores data about a Table
 *  @details Allows you to create a table in a database
 *  using a predefined schema, a list<Column>(), or on demand using add_column().
 *  Optionally, by setting exists to false in the constructor you can tell the class
 *  to add itself to the db.  exists simply tells the class whether the file on disk
 *  already has this table or not.  If exists == false, then the class will create the
 *  table and automatically add an index column, 'row' as 'INTEGER PRIMARY KEY'.  Then
 *  the rest of the columns are added if any are specified.
 */
class Table {
public:
	//! @short Creates a new table
	//! @details Will create a new table.  By default, it assumes that the table already exists
	//! in the file on disk.  Since a table must have a primary key at creation time, a key called 'row'
	//! is automatically created as part of this constructor only if exists=false.
	//! @param db A pointer to the database this table belongs to
	//! @param name The name of this table
	//! @param exists Defaults to true.  Whether this table already exists in the database on disk.
	Table(Database * db, str name, bool exists);
	//! @short Creates a new table
	//! @param db A pointer to the database this table belongs to
	//! @param name The name of this table
	//! @param columns A list of columns this table has/should have
	//! @param exists Defaults to true.  Whether this table already exists in the database on disk.
	Table(Database * db, str name, col_list & columns, bool exists = false);
	//! @short Creates a new table
	//! @details A schema looks like <code>col_name col_type, col_name2 col_type, col_name3 col_type</code>
	//! This function isn't smart enough to parse "INTEGER PRIMARY KEY" or anything beyond simple
	//! types.
	//! @param db A pointer to the database this table belongs to
	//! @param name The name of this table
	//! @param columns The table's schema
	//! @param exists Defaults to true.  Whether this table already exists in the database on disk.
	Table(Database * db, str name, bool exists, ...);
	~Table();
	//! @short Add a row to the database.
	//! @details The number of data items in the Row must equal the number of Columns in the table.
	//! This function can also be part of a transaction by setting deferred to defer_force and then
	//! calling Database::commit() when all operations are queued up.
	//! @param data The Row data to add
	//! @param deferred Whether to include this addition in a transaction
	void add_row(Row & data, defer_types_t deferred = defer_none);
	//! @short Modify a row in the database
	//! @details You should perform a direct query on the Database instance to find the rowid of the row
	//! you wish to modify
	//! @param rowid The row whose data should be replaced
	//! @param data The data to replace the row with
	//! @param deferred Whether to include this modification in a transaction
	void mod_row(int rowid, Row & data, defer_types_t deferred = defer_none);
	//! @short Add a column to the Table
	//! @details If the column doesn't exist in the database on disk, set exists to false
	//! @param column The Column to add
	//! @param exists Whether the column exists on disk or not
	void add_column(Column column, bool exists = true);
	//! @short Add a col_list to the Table
	//! @param columns The list of Columns to add
	//! @param exists Whether the columns exist on disk or not
	void add_columns(col_list & columns, bool exists = true);
	//! @short Add a variable number of columns to the table.
	//! @details Last column must be Column("null", db_type_null)
	//! @param exists Whether the columns exist on disk or not
	//! @param ... The Columns to add
	void add_columns(bool exists, ...);
	//! @short Gets the name of the table
	//! @return the name
	str get_name();
	//! @short Gets a pointer to a list<Column>
	//! @return a pointer to list<Column>
	col_list * get_columns();
	//! @short Gets the entire set of rows from the Table
	//! @return The data in the table in a list<Row>
	row_list get_rows();
	//! @short Get the last error code.
	//! @details returns 0 if no error, >0 otherwise
	//! @return the error code
	int get_error();
	//! @short Query the database using a particular query
	//! @details The parameter is inserted between 'SELECT' and 'FROM table_name'
	//! @param query The selector for sqlite
	//! @return list<Row>, null if error
	row_list query(str query);
private:
	// initialize the database with a set of columns
	void Table_init(Database *db, str name, col_list &columns, bool exists);
	Database * data_db;               // connection to the database
	int error;                        // the last error code
	int num_columns;                  // the number of columns
	str data_name;            // the name of the table
	col_list * data_columns; // the list of columns
};
/** @short A Database connection to a file.
 */
class Database {
public:
	//! @short Creates a non-initialized Database
	Database();
	//! @short Creates a Database by opening the file passed
	//! @param file The file to use as the database
	Database(str file);
	// pass in pointer to list of columns to get.  list is modified so that each entry now has
	// a Row result
	//! @short Execute a query that returns a list of results
	//! @details You must pass the string, table, and a *pointer* to a list you want the results in
	//! @param query The selector to insert between 'SELECT' and 'FROM table_name'
	//! @param table The table to get the data from
	//! @param results The pointer to a list you want results put into.  Must be initialized before calling execute_query().
	//! @return The error code, 0 if successful
	int execute_query(str query, Table & table, row_list & results);
	//! @short Execute an arbitrary sql statement optionally deferring the execution until commit() is called.
	//! @param statement The sql command to execute
	//! @param deferred Whether to defer until commit() or not
	//! @return The error code, 0 if successful
	int execute_statement(str statement, defer_types_t deferred = defer_none);
	//! @short Commit all deferred commands to the database.
	//! @return The error code, 0 if successful
	int commit();
	//! @short Clear the deferred commands list
	void rollback();
	//! @short Checks to see if a database has been loaded
	//! @return true if a db has been loaded, false otherwise
	bool is_loaded();
private:
	bool loaded;                                 // whether the db is loaded
	sqlite3x::sqlite3_connection DB;             // the db connection
	str_list deferred_statements;  // the deferred statements
};
#endif