/**
 * @file tags.h
 * @license GPLv2
 * @author Kevin Ross <r0ssar00@gmail.com> Copyright (C) 2009 Kevin Ross
 */
#ifndef _TAGS_H
#define _TAGS_H
class TagSet {
public:
	TagSet();
	TagSet(std::string file);
	void add_tag(std::string tag);
	void rm_tag(std::string tag);
	std::list<std::string> * get_tags();
	std::string get_pre();
	std::string get_post();
	std::string get_file();
	bool get_error();
private:
	void process(std::string comment);
	std::list<std::string> tags;
	std::string pre,post,file;
	bool err;
};
#endif