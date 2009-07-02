/*
 *  tagdb.cpp
 *  XTagRev
 *
 *  Created by Kevin Ross on 20/01/09.
 *
 */
using namespace std;
#include <string>
#include <list>
#include <vector>
#include <pthread.h>
#include "sqlite3x.hpp"
#include "sqlite3x_settings_db.hpp"
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>
#include <Foundation/Foundation.h>
using namespace sqlite3x;
#include "platform.h"
#include "osxutils.h"
#include "tagdb.h"

pthread_mutex_t TagDBLock;
void InitMutex() {
	pthread_mutex_init(&TagDBLock, NULL);
};
void DestroyMutex() {
	pthread_mutex_destroy(&TagDBLock);
};
sqlite3_connection TagDB;
void CreateTagDB(string arg1) {
	FsTouch(arg1);
	TagDB.open(arg1);
	TagDB.executenonquery("CREATE TABLE tagdb_table (tag_name varchar(255), tag_count int)");
	TagDB.close();
	
};
int LoadTagDB(string arg1) {
	if (FsExecuteQuery("XTagRevDatabaseFile").empty()) {
		CreateTagDB(arg1);
		SetFileComment(arg1, "@XTagRevDatabaseFile;");
	}
	list<string> file = FsExecuteQuery("XTagRevDatabaseFile");
	TagDB.open(file.empty()?arg1:file.front());
	return 0;
};
string int2str(int a) {
	std::string o = "";
	char b[4];
	sprintf(b,"%d",a);
	o+=b;
	return o;
}
void AddTagDB(string arg1) {
	vector<TagStruct> tags = GetTagDB();
	pthread_mutex_lock(&TagDBLock);
	vector<TagStruct>::iterator i;
	for (i=tags.begin(); i!=tags.end(); i++) {
		if (arg1==(*i).name) {
			pthread_mutex_unlock(&TagDBLock);
			return;
		}
	}
	list<string> count = FsExecuteQuery(arg1);
	TagDB.executenonquery(string("INSERT INTO tagdb_table VALUES (\"") + arg1 + string("\",") + int2str(count.empty()?0:count.size()) + string(")"));
	pthread_mutex_unlock(&TagDBLock);
};
void RmTagDB(string arg1) {
	pthread_mutex_lock(&TagDBLock);
	TagDB.executenonquery("DELETE FROM tagdb_table WHERE tag_name = \"" + arg1 + "\"");
	pthread_mutex_unlock(&TagDBLock);
};
vector<TagStruct> GetTagDB() {
	pthread_mutex_lock(&TagDBLock);
	vector<TagStruct> tags;
	TagStruct tmp;
	sqlite3_command tr(TagDB,"SELECT * FROM tagdb_table");
	sqlite3_cursor trc(tr.executecursor());
	while(trc.step()) {
		tmp.name=trc.getstring(0);
		tmp.count=trc.getint(1);
		tags.push_back(tmp);
	}
	pthread_mutex_unlock(&TagDBLock);
	return tags;
};
