/**
 * @file search.h
 * @license GPLv2
 * @author Kevin Ross <r0ssar00@gmail.com> Copyright (C) 2009 Kevin Ross
 */
#ifndef _SEARCH_H
#define _SEARCH_H
/** @short contains methods for querying system indexer
 */
class Search {
public:
	//! @short Creates a new Search object, no throttling
	Search();
	//! @short Creates a new Search object with defined throttling
	//! @param do_throttle Whether to throttle or not by default.
	Search(bool do_throttle = false);
	//! @short Query the search backend for do_query in any field
	//! @param do_query the search test
	//! @return the list of results as absolute pathnames
	std::list<std::string> query(std::string do_query);
	//! @short Query the search backend for do_query in the comment field
	//! @param do_query the comment to search for
	//! @return the list of results as absolute pathnames
	std::list<std::string> query_comment(std::string do_query);
	//! @short Dual purpose set/get throttle
	//! @details If called without arguments, returns the current default throttle setting.
	//! If called with a boolean argument, sets the default throttle setting and returns it.
	//! @param do_throttle The throttle setting to set the throttle to
	//! @return the current throttle value
	bool throttle(bool do_throttle = -1);
private:
	int data_throttle;
};
#endif