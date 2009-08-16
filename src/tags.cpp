/**
 * @file tags.cpp
 * @license GPLv2
 * @author Kevin Ross <r0ssar00@gmail.com> Copyright (C) 2009 Kevin Ross
 */
#include <string>
#include <list>
#include <iostream>
#include <cc++/string.h>
#include <platform/tags.h>
// the following includes include the platform-specific methods for accessing tags
// the include must have SetFileComment and GetFileComment where each uses std::string
// for values
#ifdef MACOSX
 #include "osx/osx.h"
#endif
TaggedFile::TaggedFile() {
	err = false;
	file = "";
	pre = "";
	post = "";
}
TaggedFile::TaggedFile(std::string file) {
	this->file = file;
	std::string file_tags = GetFileComment(file);
	if (file_tags=="") {
		err = true;
		return;
	}
	process(file_tags);
}
void TaggedFile::process(std::string comment) {
	std::string temp;
	if (comment == "") {
		err = true;
		return;
	}
	pre = comment.substr(0,comment.find_first_of("@")-1);
	post = comment.substr(comment.find_last_of(";")+1);
	temp = comment.substr(comment.find_first_of("@"), comment.find_last_of(";") - comment.find_first_of("@") + 1);
	while (temp != "") {
		add_tag(temp.substr(temp.find_first_of("@")+1,temp.find_first_of(";")-1));
		temp = temp.substr(temp.find_first_of(";")+1);
	}
}
void TaggedFile::add_tag(std::string tag) {
	tags.push_back(tag);
}
void TaggedFile::rm_tag(std::string tag) {
	tags.remove(tag);
}
std::list<std::string> * TaggedFile::get_tags() {
	return &tags;
}
std::string TaggedFile::get_pre() {
	return pre;
}
std::string TaggedFile::get_post() {
	return post;
}
std::string TaggedFile::get_file() {
	return file;
}
bool TaggedFile::get_error() {
	return err;
}
