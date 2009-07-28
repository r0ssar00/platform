/**
 * @file search.cpp
 * @license GPLv2
 * @author Kevin Ross <r0ssar00@gmail.com> Copyright (C) 2009 Kevin Ross
 */
#include <string>
#include <list>
#include "search.h"
//#ifdef MACOSX
#include "osx.h"
//#endif

Search::Search() {
	data_throttle = false;
}

Search::Search(bool do_throttle) {
	data_throttle = do_throttle;
}
std::list<std::string> Search::query(std::string do_query) {
	return QuerySpotlight(do_query, throttle());
}
std::list<std::string> Search::query_comment(std::string do_query) {
	return QueryComment(do_query, throttle());
}
bool Search::throttle(bool do_throttle) {
	if (do_throttle==-1) {
		return data_throttle;
	} else {
		data_throttle = do_throttle;
		return data_throttle;
	}
}