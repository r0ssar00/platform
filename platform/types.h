/*
 *  types.h
 *  platform
 *
 *  Created by Kevin Ross on 09/08/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H
#include <vector>
#include <string>

/**
 * @short the string typedefs
 */
typedef std::string str;
typedef std::vector<str> str_list;
typedef str_list::iterator str_list_i;

// only include them if using the database
#ifdef PLATFORM_DATABASE_H
class Database;
class Table;
class Column;
class Row;
class Object;

/**
 * @short the database typedefs
 */
typedef std::vector<Column> col_list;
typedef col_list::iterator col_list_i;
typedef std::vector<Row> row_list;
typedef row_list::iterator row_list_i;
typedef std::vector<Object> obj_list;
typedef obj_list::iterator obj_list_i;
typedef std::map<Column, Object> col_obj_map;
typedef std::pair<Column, Object> col_obj_pair;
#endif
#endif