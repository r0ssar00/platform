/**
 * @file test.cpp
 * @license GPLv2
 * @author Kevin Ross <r0ssar00@gmail.com> Copyright (C) 2009 Kevin Ross
 * @short Simple testing program to help me test classes
 */
#include <string>
#include <list>
#include <map>
#include <vector>
#include <iostream>
#include <platform/database.h>
#define N_ELEMS( a ) (sizeof(a)/sizeof(*a)) 

int main(int argc, char *argv[]) {
	std::cout << "Creating database @ " << argv[1] << std::endl;
	std::string dbfile = argv[1];
	Database hello(dbfile);
	std::cout << "Adding table hello_world" << std::endl;
	Table tbl(&hello, "hello_world", false, "col12345 TEXT", "col2 INTEGER", "col3 TEXT", "col4 INTEGER", "NULL");
	std::cout << "Creating row 1" << std::endl;
	Row * data = new Row();
	data->add("col12345", Object("col1_row1"))->add("col2", Object(1))->add("col3", Object("col3_row1"))->add("col4", Object(4));
	std::cout << "Adding row 1" << std::endl;
	tbl.add_row(*data);
	delete data;
	std::cout << "Creating row 2" << std::endl;
	data = new Row();
	data->add("col12345", Object("col1_row2"))->add("col2", Object(2))->add("col3", Object("col3_row2"))->add("col4", Object(5));
	std::cout << "Adding row 2" << std::endl;
	tbl.add_row(*data);
	delete data;
	std::cout << "Printing table to stdout" << std::endl;
	std::cout << tbl.to_string() << std::endl;
}
