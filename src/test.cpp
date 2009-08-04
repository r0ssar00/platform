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
#include <platform/tags.h>
#include <platform/database.h>
#define N_ELEMS( a ) (sizeof(a)/sizeof(*a)) 
// Using database:
enum tablename_t {
	tablename_Column1 = 0,
	tablename_Column2,
	tablename_Column3
};
// then 
int main(int argc, char *argv[]) {
	std::string file("/Users/r0ssar00/Binaries/cliclick");
	TagSet * output;
	output = new TagSet(file);
	bool err = output->get_error();
	if (err) {
		std::cout << "Error getting tags!" << std::endl;
	}
	std::list<std::string> * results = output->get_tags();
	for (std::list<std::string>::iterator i = results->begin(); i != results->end(); i++) {
		std::cout << "Tag: " << *i << std::endl;
	}
	std::cout << "Creating database @ " << argv[1] << std::endl;
	std::string dbfile = argv[1];
	Database hello(dbfile);
	Table (hello, "hello_world", false, "col1 TEXT", "col2 INTEGER", "col3 TEXT", "col4 TEXT", "col5 INTEGER", "NULL");
}
