/*
 *  main.h
 *  XTagRev
 *
 *  Created by Kevin Ross on 07/02/09.
 *  Copyright 2009 uOttawa. All rights reserved.
 *
 */
#include <dbus-c++/dbus.h>
#include "server-glue.h"
#include "settingsdb.h"
static std::string int2str(int a) {
	std::string o = "";
	char b[4];
	sprintf(b,"%d",a);
	o+=b;
	return o;
};
class TagDBsrv : public com::ross::XTagRev::TagDB_adaptor, public DBus::IntrospectableAdaptor, public DBus::ObjectAdaptor {
public:
	TagDBsrv(DBus::Connection &connection) : ObjectAdaptor(connection, "/com/ross/XTagRev/TagDB") {}
	void AddTag(const std::string& tagname) {
		AddTagDB(tagname);
	}
	void RmTag(const std::string& tagname) {
		RmTagDB(string(tagname));
	}
	bool LoadTags(const std::string& filename) {
		return LoadTagDB(filename);
	}
	vector< std::string > GetTagNames() {
		vector<string> out;
		vector<TagStruct> tags = GetTagDB();
		vector<TagStruct>::iterator i;
		for(i=tags.begin();i!=tags.end();i++) {
			out.push_back(i->name);
		}
		return out;
	}
	vector< std::string > GetTagSizes() {
		vector<string> out;
		vector<TagStruct> tags = GetTagDB();
		vector<TagStruct>::iterator i;
		for(i=tags.begin();i!=tags.end();i++) {
			out.push_back(int2str(i->count));
		}
		return out;
	}
	void ForceReloadTagDB()  {
		ReloadTagDB();
	}
};
class Platformsrv : public com::ross::platform_adaptor, public DBus::IntrospectableAdaptor, public DBus::ObjectAdaptor {
public:
	Platformsrv(DBus::Connection &connection) : ObjectAdaptor(connection, "/com/ross/platform") {}
    string GetTag(const std::string& filename)  {
		return FsGetTag(filename);
	}
    vector< std::string > GetTags(const std::string& filename) {
		return FsGetTags(filename);
	}
    int32_t SetTag(const std::string& filename, const std::string& tag) {
		return FsSetTag(filename, tag);
	}
    int32_t AddTag(const std::string& filename, const std::string& tag) {
		return FsAddTag(filename, tag);
	}
    int32_t RmTag(const std::string& filename, const std::string& tag) {
		return FsRmTag(filename, tag);
	}
    vector< std::string > Query(const std::string& query, const bool& throttle) {
		return ListToVector(FsQuery(query));
	}
    vector< std::string > ExecuteQuery(const std::string& query, const bool& throttle) {
		return ListToVector(FsExecuteQuery(query));
	}
};
class Filesystemsrv : public com::ross::filesystem_adaptor, public DBus::IntrospectableAdaptor, public DBus::ObjectAdaptor {
public:
	Filesystemsrv(DBus::Connection &connection) : ObjectAdaptor(connection, "/com/ross/filesystem") {}
	vector< std::string > Cat(const std::string& filename) {
		return ListToVector(FsCat(filename));
	}
    int32_t Rm(const std::string& filename) {
		return FsRm(filename);
	}
    int32_t RmDir(const std::string& foldername) {
		return FsRmDir(foldername);
	}
    vector< std::string > Ls(const std::string& foldername) {
		return ListToVector(FsLs(foldername));
	}
    int32_t MkDir(const std::string& foldername) {
		return FsMkDir(foldername);
	}
    int32_t Cp(const std::string& filefrom, const std::string& fileto) {
		return FsCp(filefrom, fileto);
	}
    int32_t CpDir(const std::string& folderfrom, const std::string& folderto) {
		return FsCpDir(folderfrom, folderto);
	}
    int32_t Touch(const std::string& filename) {
		return FsTouch(filename);
	}
    vector< std::string > Find(const std::string& foldername, const bool& throttle) {
		list<string> out;
		FsFind(foldername,&out,throttle);
		return ListToVector(out);
	}
    vector< std::string > FindFiles(const std::string& foldername, const bool& throttle) {
		list<string> out;
		FsFindFiles(foldername,&out,throttle);
		return ListToVector(out);
	}
    vector< std::string > FindFolders(const std::string& foldername, const bool& throttle) {
		list<string> out;
		FsFindFolders(foldername,&out,throttle);
		return ListToVector(out);
	}
    string Basename(const std::string& itemname) {
		return FsBasename(itemname);
	}
    bool Exists(const std::string& itemname) {
		return FsExists(itemname);
	}
    string GetLinkType(const std::string& itemname) {
		return FsGetLinkType(itemname);
	}
	int32_t MakeAlias(const std::string& from, const std::string& to, const std::string& kind) {
		return FsLn(from,to,kind);
	}
	string ResolveAlias(const std::string& path) {
		return FsResolveAlias(path);
	}
	// allows me to link a whole bunch of files at once so I avoid many calls
	// takes 3 arrays: list of files from, corresponding list of files to, and corresponding alias type
	void LinkGroup(const std::vector< std::string >& from, const std::vector< std::string >& to, const std::vector< std::string >& kind) {
		vector<string> fromr = from;
		vector<string> tor = to;
		vector<string> kindr = kind;
		vector<string>::iterator i = fromr.begin();
		vector<string>::iterator j = tor.begin();
		vector<string>::iterator k = kindr.begin();
		while (i!=fromr.end()) {
			MakeAlias(*i,*j,*k);
			i++;
			j++;
			k++;
		}
	}
};

class Settingssrv : public com::ross::settings_adaptor, public DBus::IntrospectableAdaptor, public DBus::ObjectAdaptor {
public:
	Settingssrv(DBus::Connection &connection) : ObjectAdaptor(connection, "/com/ross/settings") {}
	void LoadSettings(const std::string& from) {
		datafile=from;
		MySettings.LoadSettings(from);
	}
	void SaveSettings(const std::string& to) {
		MySettings.SaveSettings(to==""?datafile:to);
	}
	string GetSetting(const std::string& key) {
		return MySettings.GetSetting(key);
	}
	void SetSetting(const std::string& key, const std::string& value, const bool& save) {
		MySettings.SetSetting(key,value);
		if (save) MySettings.SaveSettings(datafile);
	}
	void DeleteSetting(const std::string& key) {
		MySettings.DeleteSetting(key);
	}
	map<string, string> GetRaw() {
		return MySettings.GetMap();
	}
	void ForceReloadSettings() {
		ReloadSettings();
	}
private:
	string datafile;
	SettingsDb MySettings;
};

class Loggersrv : public com::ross::logger_adaptor, public DBus::IntrospectableAdaptor, public DBus::ObjectAdaptor {
public:
	Loggersrv(DBus::Connection &connection) : ObjectAdaptor(connection, "/com/ross/logger") {LogLevel=0;}
	void Log(const string &who, const string &what, const int32_t &level=0) {
		if (level <=LogLevel) {
			Logging(who + ": " + what);
		}
		DoLog(who,what,level);
	}
	void Level(const int32_t &level) {
		LogLevel = level;
	}
	int32_t GetLevel() {
		return LogLevel;
	}
private:
	int LogLevel;
};