/** @file database.h
 *  @license GPLv2
 *  @author Kevin Ross <r0ssar00@gmail.com> Copyright (C) 2009 Kevin Ross
 *  @short Encapsulates database access in easy to use objects
 *
 *  @details Allows really easy access to an sqlite database without having to worry
 *  about sqlite statements.  Core class is the Database.  You create Tables associated
 *  with a database to which you can add Columns and then Rows.
 */
#include <sqlite3x.hpp>
/**
 * @short the column type enumeration
 */
enum db_types_t {
	//! integer type
	db_type_int = 0,
	//! string type
	db_type_str
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

/** @short The core data storage object
 *  @details The Object stores several datatypes(described in db_types_t).  You will use
 *  this class heavily whenever developing a program using the database.
 */
class Object {
public:
	//! @short Creates a string Object
	//! @param str The string to initialize the Object with
	Object(std::string str);
	//! @short Creates an int Object
	//! @param integer The int to init the Object with
	Object(int integer);
	/** @short gets the type of the Object
	 *  @return the type
	 */
	db_types_t get_type();
	/** @short gets whether the Object is an int
	 *  @return true if int, false otherwise
	 */
	bool is_int();
	/** @short gets whether the Object is a string
	 *  @return true if string, false otherwise
	 */
	bool is_string();
	/** @short returns the internal data as a string.
	 *  @detail if the Object represents an int, this
	 *  will return the string representation of the int, eg int(5)=string("5")
	 *  @return string value of internal data
	 */
	std::string as_string();
	/** @short returns the internal data as int.
	 *  @detail this will NOT convert the string
	 *  into any form.  it will fail.
	 *  @return int value of internal data
	 */
	int as_int();
private:
	std::string data_str; // stores the string data
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
	Row(std::list<Object> & data);
	//! @short Gets an iterator pointing to the first column's data
	//! @return an interator
	std::list<Object>::iterator begin();
	//! @short Gets an iterator pointing to the last column's data
	//! @return an iterator
	std::list<Object>::iterator end();
	//! @short Get the number of columns this row has
	//! @return the number of columns
	int column_count();
private:
	std::list<Object> dataset; // stores the column data
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
	//! @short Creates a new column with a name and type
	//! @param name The name of the column
	//! @param type The type of the column
	Column(std::string name, db_types_t type);
	//! @short Creates a new column with a name, type, and index.
	//! @details Using this constructor means you have to
	//! keep track of the number of columns manually.
	//! Don't use it because it's a whole lot easier to
	//! use Column(std::string, db_types_t).
	//! @param name The name of the column
	//! @param type The type of the column
	//! @param index The index of the column
	Column(std::string name, db_types_t type, int index);
	//! @short Get the data type of the Column
	//! @return the type
	db_types_t get_type();
	//! @short Get the name of the Column
	//! @return the name
	std::string get_name();
	//! @short Get the index of the Column
	//! @details This is used to get this column's index.
	//! You won't need to use it.  It's here for the Table
	//! and Database classes to use
	//! @return the column's index
	int get_index();
private:
	std::string data_name; // stores the column name
	db_types_t data_type;  // stores the column tyoe
	int data_index;        // stores the column index
};
/** @short Stores data about a Table
 *  @details Allows you to create a table in a database
 *  using a predefined schema, a list<Column>(), or on demand using add_column().
 *  Optionally, by setting exists to false in the constructor you can tell the class
 *  to add itself to the db.  exists simply tells the class whether the file on disk
 *  already has this table or not.
 */
class Table {
public:
	//! @short Creates a new table
	//! @param db A pointer to the database this table belongs to
	//! @param name The name of this table
	//! @param exists Defaults to true.  Whether this table already exists in the database on disk.
	Table(Database * db, std::string name, bool exists = true);
	//! @short Creates a new table
	//! @param db A pointer to the database this table belongs to
	//! @param name The name of this table
	//! @param columns A list of columns this table has/should have
	//! @param exists Defaults to true.  Whether this table already exists in the database on disk.
	Table(Database * db, std::string name, std::list<Column> & columns, bool exists = false);
	//! @short Creates a new table
	//! @details A schema looks like <code>col_name col_type, col_name2 col_type, col_name3 col_type</code>
	//! @param db A pointer to the database this table belongs to
	//! @param name The name of this table
	//! @param columns The table's schema
	//! @param exists Defaults to true.  Whether this table already exists in the database on disk.
	Table(Database * db, std::string name, std::string columns, bool exists = false);
	~Table();
	//! @short Add a row to the database.
	//! @details The number of data items in the Row must equal the number of Columns in the table.
	//! This function can also be part of a transaction by setting deferred to defer_force and then
	//! calling Database::commit() when all operations are queued up.
	//! @param data The Row data to add
	//! @param deferred Whether to include this addition in a transaction
	void add_row(Row & data, defer_types_t deferred = defer_none);
	//! @short Add a column to the Table
	//! @details If the column doesn't exist in the database on disk, set exists to false
	//! @param column The Column to add
	//! @param exists Whether the column exists on disk or not
	void add_column(Column column, bool exists = true);
	//! @short Add a std::list<Column> to the Table
	//! @param columns The list of Columns to add
	//! @param exists Whether the columns exist on disk or not
	void add_columns(std::list<Column> & columns, bool exists = true);
	//! @short Gets the name of the table
	//! @return the name
	std::string get_name();
	//! @short Gets a pointer to a list<Column>
	//! @return a pointer to list<Column>
	std::list<Column> * get_columns();
	//! @short Gets the entire set of rows from the Table
	//! @return The data in the table in a list<Row>
	std::list<Row> get_rows();
	//! @short Get the last error code.
	//! @details returns 0 if no error, >0 otherwise
	//! @return the error code
	int get_error();
	//! @short Query the database using a particular query
	//! @details The parameter is inserted between 'SELECT' and 'FROM table_name'
	//! @param query The selector for sqlite
	//! @return list<Row>, null if error
	std::list<Row> query(std::string query);
private:
	// initialize the database with a set of columns
	void Table_init(Database *db, std::string name, std::list<Column> &columns, bool exists);
	Database * data_db;               // connection to the database
	int error;                        // the last error code
	int num_columns;                  // the number of columns
	std::string data_name;            // the name of the table
	std::list<Column> * data_columns; // the list of columns
};
/** @short A Database connection to a file.
 */
class Database {
public:
	//! @short Creates a non-initialized Database
	Database();
	//! @short Creates a Database by opening the file passed
	//! @param file The file to use as the database
	Database(std::string file);
	// pass in pointer to list of columns to get.  list is modified so that each entry now has
	// a Row result
	//! @short Execute a query that returns a list of results
	//! @details You must pass the string, table, and a *pointer* to a list you want the results in
	//! @param query The selector to insert between 'SELECT' and 'FROM table_name'
	//! @param table The table to get the data from
	//! @param results The pointer to a list you want results put into.  Must be initialized before calling execute_query().
	//! @return The error code, 0 if successful
	int execute_query(std::string query, Table & table, std::list<Row> *& results);
	//! @short Execute an arbitrary sql statement optionally deferring the execution until commit() is called.
	//! @param statement The sql command to execute
	//! @param deferred Whether to defer until commit() or not
	//! @return The error code, 0 if successful
	int execute_statement(std::string statement, defer_types_t deferred = defer_none);
	//! @short Commit all deferred commands to the database.
	//! @return The error code, 0 if successful
	int commit();
	//! @short Clear the deferred commands list
	void rollback();
	//! @short Checks to see if a database has been loaded
	//! @return true if a db has been loaded, false otherwise
	bool is_loaded();
private:
	int locked;                                  // whether the db is locked
	bool loaded;                                 // whether the db is loaded
	sqlite3x::sqlite3_connection DB;             // the db connection
	std::list<std::string> deferred_statements;  // the deferred statements
	void lock();                                 // lock the database for r/w.  if locked, blocks calling function until unlocked
	void unlock();                               // unlock the database
};
