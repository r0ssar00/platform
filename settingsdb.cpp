/*
 *  setingsdb.cpp
 *  xtagrev
 *
 *  Created by Kevin Ross on 10/02/09.
 *  Copyright 2009 uOttawa. All rights reserved.
 *
 */
using namespace std;
#include <iostream>
#include <string>
#include <c++/4.0.0/map>
#include "settingsdb.h"
#include "platform.h"

void SettingsDb::LoadSettings(string arg1) {
	string file = "";
	if (arg1=="") {
		file=string(getenv("HOME")) + "/.com.ross.settingsdb";
	} else {
		file=arg1;
	}
	list<string> lines;
	if (!FsExists(file)) {
		return;
	} else {
		lines = FsCat(file);
	}
	lines.sort();
	string key, value;
	list<string>::iterator i;
	for (i=lines.begin(); i!=lines.end(); i++) {
		key=i->substr(0,i->find_first_of("="));
		value=i->substr(i->find_first_of("=") + 1);
		settings[key]=value;
	}
}
void SettingsDb::SaveSettings(string arg1) {
	string file = "";
	if (arg1=="") {
		file=string(getenv("HOME")) + "/.com.ross.settingsdb";
	} else {
		file=arg1;
	}
	if (FsExists(file)) {
		FsRm(file);
	} 
	FsTouch(file);
	list<string> lines;
	for (map<string, string>::iterator i=settings.begin(); i != settings.end() ; i++) {
		lines.push_back(i->first + "=" + i->second);
	}
	lines.sort();
	FsEcho(lines,file,false);
}
string SettingsDb::GetSetting(string key) {
	if (settings.count(key)==0) {
		return "";
	}
	return settings[key];
}
void SettingsDb::SetSetting(string key, string value) {
	settings[key]=value;
}
void SettingsDb::DeleteSetting(string key) {
	settings.erase(key);
}