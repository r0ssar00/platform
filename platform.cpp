/*
 *  platform.cpp
 *  XTagRev
 *
 *  Created by Kevin Ross on 20/01/09.
 *
 */

using namespace std;
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sysexits.h>
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <Foundation/Foundation.h>
#include <time.h>
#include "osxutils.h"
#include "platform.h"

vector<string> FsGetTags(string filename) {
	string tags=FsGetTag(filename);
	if (tags == "") {
		return vector<string>();
	}
	// tags looks like "some stuff@a tag;@another tag;@a third tag;some more stuff
	int fi = tags.find_first_of("@");
	int ls = tags.find_last_of(";");
	string rtags = tags.substr(fi,tags.length()-ls);
	vector<string> out;
	// now get the pre stuff and put it into the 1st entry.  if none, push a blank onto it
	if (fi != 0) {
		out.push_back(rtags.substr(0,fi-1));
	} else {
		out.push_back("");
	}
	// and the post stuff
	if (ls != tags.length()) {
		out.push_back(tags.substr(ls+1));
	} else {
		out.push_back("");
	}
	// rtags looks like "@a tag;@another tag;@a third tag;"
	while (rtags!="") {
		cout << rtags <<endl;
		out.push_back(rtags.substr(rtags.find_first_of("@")+1,rtags.find_first_of(";")-1));
		rtags = rtags.substr(rtags.find_first_of(";")+1);
	}
	return out;
}
string FsGetLinkType(string filename) {
	// one of my tags looks like @tag name;@another tag;kind=a
	string tags=FsGetTag(filename);
	if (tags.find("kind=") == string::npos) {
		return "";
	}
	if (tags.substr(tags.find("kind=")+5,1)=="a") {
		return "alias";
	} else if (tags.substr(tags.find("kind")+5,1)=="l") {
		return "link";
	} else {
		return "";
	}
}
list<string> FsCat(string arg1) {
	list<string> out;
	ifstream input(arg1.c_str());
	string c;
	if (input.is_open()) {
		while (!input.eof()) {
			getline(input,c);
			out.push_back(c);
		}
	}
	return out;
};

int FsWc(list<string> arg1) {
	return (arg1.size()>0)?arg1.size() - 1:0;
};
int FsWc(string arg1) {
	return FsWc(FsCat(arg1));
};

list<string> FsHead(list<string> arg1, int c) {
	list<string>::iterator i;
	list<string> out;
	i=arg1.begin();
	for (;c>0; c--) {
		out.push_back(*i);
		i++;
	}
	return out;
};

list<string> FsTail(list<string> arg1, int c) {
	list<string>::iterator i;
	list<string> out;
	list<string> temp;
	list<string>::iterator j;
	for(i=arg1.begin(); i!=arg1.end(); i++) {
		temp.push_back(*i);
	}
	j=temp.end();
	for(;c>0; c--) {
		i--;
		temp.push_front(*i);
	}
	for(j=temp.begin();j!=temp.end();j++) {
		out.push_back(*j);
	}
	return out;
};

int FsRm(string arg1) {
	return remove(arg1.c_str());
};

int FsRmDir(string arg1) {
	return removedirectoryrecursively(arg1);
};

list<string> FsLs(string arg1) {
	list<string> out;
	FsFindFiles(arg1,&out,false,false,false);
	return out;
};

void FsFindFiles(string arg1, list<string> *out, bool throttle, bool folders, bool recurse) {
	DIR *dir;
	struct dirent *entry;
	string path;
	dir = opendir(arg1.c_str());
	if (dir==NULL) {
		return;
	}
	// get rid of . and ..
	readdir(dir);
	readdir(dir);
	while ((entry = readdir(dir)) != NULL) {
		if (throttle) {
			usleep(10000);
		}
		if (entry->d_type == DT_DIR && (entry->d_name != "." || entry->d_name != "..")) {
			if (folders) {
				out->push_back(arg1+string("/")+string(entry->d_name));
			}
			if (recurse) {
				FsFindFiles(arg1+string("/")+string(entry->d_name),out,throttle,folders);
			}
		}
		if (!folders) {
			if (entry->d_type != DT_DIR && (entry->d_name != "." || entry->d_name != "..")) {
				out->push_back(arg1+string("/")+string(entry->d_name));
			}
		}
    }
	closedir(dir);
};

void FsFindFolders(string arg1, list<string> * out, bool throttle) {
	FsFindFiles(arg1,out,throttle,true);
};

void FsFind(string arg1, list<string> * out, bool throttle) {
	FsFindFiles(arg1, out, throttle);
	FsFindFolders(arg1, out, throttle);
	out->sort();
};

int FsLn(string arg1, string arg2, string kind) {
	if (access(arg1.c_str(),F_OK)==-1) {
		return 0;
	}
	if (access(arg2.c_str(),F_OK) != -1) {
		return 0;
	}
	if (kind=="") {
		return 0;
	}
	if (kind=="alias") {
		int err = CreateAlias(arg1.c_str(),arg2.c_str());
		if (err != 0) {
			return 0;
		}
		struct stat h;
		stat(arg1.c_str(),&h);
		// make it available from the command-line by letting one do file="`bash an_alias -echo-alias`"
		list<string> bash;
		bash.push_back("#!/bin/bash");
		// only make it executable if the original file is
		if (((h.st_mode & S_IXUSR) == S_IXUSR || (h.st_mode & S_IXGRP) == S_IXGRP || (h.st_mode & S_IXOTH) == S_IXOTH) &&
			!((h.st_mode & S_IFDIR) == S_IFDIR)) { // only if not a directory
			// allow you to run it optionally allowing you to see where it points
			bash.push_back("if [ $1 ]; then");
			bash.push_back("	if [ \"$1\" = \"-resolve-alias\" ]; then");
			bash.push_back("		echo \"`dbus-send-nostats --dest=com.ross --print-reply /com/ross/filesystem com.ross.filesystem.ResolveAlias string:\"$0\"`\";");
			bash.push_back("	else");
			bash.push_back("		`dbus-send-nostats --dest=com.ross --print-reply /com/ross/filesystem com.ross.filesystem.ResolveAlias string:\"$0\"` $@");
			bash.push_back("	fi;");
			bash.push_back("else");
			bash.push_back("	`dbus-send-nostats --dest=com.ross --print-reply /com/ross/filesystem com.ross.filesystem.ResolveAlias string:\"$0\"` $@;");
			bash.push_back("fi");
			FsEcho(bash,arg2,true);
			chmod(arg2.c_str(),S_IRWXU);
		} else if (((h.st_mode & S_IXUSR) == S_IXUSR || (h.st_mode & S_IXGRP) == S_IXGRP || (h.st_mode & S_IXOTH) == S_IXOTH) &&
				   ((h.st_mode & S_IFDIR) == S_IFDIR)) {
			bash.push_back("echo \"`dbus-send-nostats --dest=com.ross --print-reply /com/ross/filesystem com.ross.filesystem.ResolveAlias string:\"$0\"`\"");
			FsEcho(bash,arg2,true);			
			// since it's a directory, make it easy to cd into by doing cd "`./name_of_dir`"
			chmod(arg2.c_str(),S_IRWXU);
		} else {
			// just show where it points when bash runs it
			bash.push_back("echo \"`dbus-send-nostats --dest=com.ross --print-reply /com/ross/filesystem com.ross.filesystem.ResolveAlias string:\"$0\"`\"");
			FsEcho(bash,arg2,true);
		}
	} else {
		return !symlink(arg1.c_str(),arg2.c_str());
	}
	return 1;
};

int FsCd(string arg1) {
	return chdir(arg1.c_str());
};

int FsMkDir(string arg1) {
	string md="mkdir -p \"";
	string en="\"";
	string wh=md + arg1 + en;
	return system(wh.c_str());
};

int FsCp(string arg1, string arg2) {
	ifstream ifs(arg1.c_str(), ios::binary);
	ofstream ofs(arg2.c_str(), ios::binary);
	if (!ifs.is_open() || !ofs.is_open()) {
		return 1;
	}
	ofs << ifs.rdbuf();
	ifs.close();
	ofs.close();
	return 0;
};

int FsCpDir(string arg1, string arg2) {
	list<string>::iterator i;
	list<string> dirs;
	list<string>::iterator j;
	list<string> files;
	FsFindFolders(arg1,&dirs);
	for (i=dirs.begin(); i!=dirs.end(); i++) {
		FsMkDir(*i);
	}
	FsFindFiles(arg1,&dirs);
	for (i=files.begin(); i!=files.end(); i++) {
		FsCp(*i,arg2 + (*i).substr(0,arg1.length()));
	}
	return 0;
};

int FsTouch(string arg1) {
	fstream ofs(arg1.c_str(), ios::out);
	if (!ofs.is_open()) {
		return 1;
	}
	ofs.close();
	return 0;
};

void FsEcho(string arg1) {
	cout << arg1 << endl;
};

int FsEcho(string arg1, string arg2, bool append) {
	fstream fout;
	fout.open(arg2.c_str(), ios::out | append ? ios::app : ios::trunc);
	if (!fout.is_open()) {
		return 1;
	}
	fout << arg1 << endl;
	fout.close();
	return 0;
};

int FsEcho(list<string> arg1, string arg2, bool append) {
	ofstream fout;
	//fout.open(arg2.c_str(), ios::out | append ? ios::app : ios::trunc);
	fout.open(arg2.c_str(), append ? ios::app : ios::trunc);
	if (!fout.is_open()) {
		return 1;
	}
	list<string>::iterator i;
	for (i=arg1.begin(); i != arg1.end(); i++) {
		// need to hack this for settings. :(
		if (*i!="=") {
			fout << (*i) << endl;
		}
	}
	fout.close();
	return 0;
};

string FsGetTag(string arg1) {
	return GetFileComment(arg1);
};

int FsSetTag(string arg1, string arg2) {
	return SetFileComment(arg1, arg2);
};

int FsAddTag(string arg1, string arg2) {
	vector<string> tag = FsGetTags(arg1);
	string outtag = "";
	tag.push_back(arg2);
	outtag+=tag[0];
	for (int i = 2; i < tag.size(); i++) {
		outtag+="@" + tag[i] + ";";
	}
	if (arg2!="") {
		outtag+="@" + arg2 + ";";
	}
	outtag+=tag[1];
	return FsSetTag(arg1,outtag);
};

int FsRmTag(string arg1, string arg2) {
	vector<string> tag = FsGetTags(arg1);
	vector<string> out;
	for (int i = 2; i < tag.size(); i++) {
		if (tag[i]!=arg2) {
			out.push_back(tag[i]);
		}
	}
	string outtag = "";
	outtag+=tag[0];
	for(int i = 0; i < out.size(); i++) {
		outtag+="@" + out[i] + ";";
	}
	outtag+=tag[1];
	return FsSetTag(arg1,outtag);
}
list<string> FsQuery(string arg1, bool throttle) {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	list<string> out;
	int count;
	NSString *Tag = [NSString stringWithUTF8String:arg1.c_str()];
	MDQueryRef query = MDQueryCreate(kCFAllocatorDefault,(CFStringRef)Tag,NULL,NULL);
	if (query == NULL) {
		return out;
	}
	Boolean ok = MDQueryExecute(query, kMDQuerySynchronous);
	if (!ok) {
		CFRelease(query);
		return out;
	}
	count = MDQueryGetResultCount(query);
	for (int i = 0; i < count; i++) {
		if (throttle) {
			usleep(10000);
		}
		MDItemRef item = (MDItemRef)MDQueryGetResultAtIndex(query, i);
		out.push_back(([(NSString *)MDItemCopyAttribute(item, kMDItemPath) UTF8String]));
	}
	CFRelease(query);
	// from NSString to string: string a = string([Tag UTF8String]);
	[pool drain];
	return out;
}
list<string> FsExecuteQuery(string arg1, bool throttle) {
	return FsQuery(string(("kMDItemFinderComment == \"*@") + arg1 + ";*\""),throttle);
};

string FsBasename(string arg1) {
	return arg1.substr(arg1.find_last_of("/")+1);
}

string FsResolveAlias(string arg1) {
	return Resolve(arg1);
}
int removedirectoryrecursively(string dirname){
	DIR *dir;
	struct dirent *entry;
	dir = opendir(dirname.c_str());
	if (dir == NULL) {
		return 1;
	}                               
	while ((entry = readdir(dir)) != NULL) {
		if (!(string(entry->d_name).compare(".") || string(entry->d_name).compare(".."))) {
			if (entry->d_type == DT_DIR) {
				removedirectoryrecursively(string(entry->d_name));
			}
			if (entry->d_type == DT_DIR) {
				rmdir(entry->d_name);
			} else {
				FsRm(string(entry->d_name));
			}
		}
		
	}
	closedir(dir);
	rmdir(dirname.c_str());
	return 0;
};
bool FsExists(string arg1) {
	struct stat stFileInfo;
	int intStat;
	intStat = stat(arg1.c_str(),&stFileInfo);
	if (intStat == 0) {
		return true;
	} else {
		return false;
	}
}
vector<string> ListToVector(list<string> l) {
	vector<string> out;
	for (list<string>::iterator i = l.begin(); i != l.end(); i++) {
		out.push_back(*i);
	}
	return out;
}
void Logging(string arg1) {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString * thestr = [NSString stringWithUTF8String:arg1.c_str()];
	NSLog(thestr);
	[pool drain];
}
string FsAppData() {
	return FsAppData("");
}
string FsAppData(string file) {
	string out = getenv("HOME");
#ifdef MACOSX
	out+="/Library/Application Support/XTagRev/";
#endif
	out+=file;
	return out;
}