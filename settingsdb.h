/*
 *  setingsdb.h
 *  xtagrev
 *
 *  Created by Kevin Ross on 10/02/09.
 *  Copyright 2009 uOttawa. All rights reserved.
 *
 */
#ifndef SETTINGSDB_H
#define SETTINGSDB_H
#include <c++/4.0.0/map>
class SettingsDb {
public:
	void LoadSettings(string arg1);
	void SaveSettings(string arg1);
	string GetSetting(string arg1);
	void SetSetting(string key, string value);
	void DeleteSetting(string key);
	map<string, string> GetMap() {return settings;}
private:
	map<string, string> settings;
};

#endif