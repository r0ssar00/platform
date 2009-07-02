// stl imports
using namespace std;
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include <cstdio>

// platform imports
// mac imports
#include "platform.h"
#include "tagdb.h"
//#include "osxutils.h"

// library imports
#include <dbus-c++/dbus.h>  //dbus

// dbus imports
#include "server-glue.h"

using namespace DBus;

// Note: executing an alias will return it's path, useful for binaries that are aliased

#pragma mark Prototypes
// Recursive Find Declarations
list<string> RecurseDir(int arg1);
string NumSpaces(int arg1);
// end Recursive Find Declarations

// String Operations
list<string> string_parse_at_semicolon(string arg1);
// substring builtin
// firstindexof builtin
// lastindexof builtin
// contains builtin
// length builtin
string string_basename(string arg1);
string string_dirname(string arg1);
string string_trimend(string arg1);
// debug functions
void debug_print(string arg1);
void debug_enable();
void debug_disable();
// End String Operations

// Tag Operations Declarations
list<string> * GetTagsFromFile(string arg1);
list<string> * GetFilesFromTag(string arg1);
// end Tag Operations Declarations

// Update Operation Declarations
// threads
int FullUpdateThread();
int FileCountThread();
int PruneLinkThread();
int NewFilesThread();
pthread_mutex_t TagsLock;
// helper functions
void CheckFileCount();
void ReBuildTagFolder();
void BuildBasicFolderTree();
void LinkFilesForTag(string arg1);
void LinkAllFiles();
void PruneTagDir(string arg1);
void LinkSingleFile(string arg1);
void LinkGroupOfFiles(string arg1);
// End Update Operation Declarations

void SleepSecond();
void SleepMilliSecond();
// begin global list of tags, files, files with tags, files in tags dir variables
list<TagStruct> Tags;
list<string> FilesForTag;
list<string> FilesWithTag;
list<string> FilesInDir;
// end global list of variables
string TagDBpath;
string TagFolder;
// end global list of variables

/* how to sleep
 #include <time.h>
 
 int main() {
 struct timespec tv;
 tv.tv_sec=1;  // or tv.tv_msec=1000;
 nanosleep(&tv, NULL);
 }
 */ 
static std::string int2str(int a) {
	std::string o = "";
	char b[4];
	sprintf(b,"%d",a);
	o+=b;
	return o;
}
void *doadd(void *ptr) {
	LoadTagDB("/Users/r0ssar00/Development/Git/XTagRev/TagDB.sqlite3");
	AddTagDB("School/Year 1/ITI1121");
	AddTagDB("Hello World 2");
	FsLn("/Users/r0ssar00/Binaries/safesleep","/Users/r0ssar00/safesleep", "alias");
	// fix warning by returning void* through a cast
	int i = 0;
	return (void*)i;
}

#include "main.h"
TagDBsrv::TagDBsrv(DBus::Connection &connection) : ObjectAdaptor(connection, "com.ross.XTagRev.TagDB") {}
void TagDBsrv::AddTag(const std::string& tagname) {
		AddTagDB(tagname);
}
void TagDBsrv::RmTag(const std::string& tagname) {
		RmTagDB(string(tagname));
}
vector<string> TagDBsrv::GetTagNames() {
		vector<string> out;
		vector<TagStruct> tags;
		vector<TagStruct>::iterator i;
		for(i=tags.begin();i!=tags.end();i++) {
			out.push_back(i->name);
		}
		return out;
}
vector<string> TagDBsrv::GetTagSizes() {
		vector<string> out;
		vector<TagStruct> tags;
		vector<TagStruct>::iterator i;
		for(i=tags.begin();i!=tags.end();i++) {
			out.push_back(int2str(i->count));
		}
	return out;
}
DBus::BusDispatcher dispatcher;
void niam(int sig)
{
	dispatcher.leave();
}

int main (int argc, char * const argv[]) {
    // insert code here...
	/*
    std::cout << "Hello, World!\n";
	list<string> f = FsCat("/Users/r0ssar00/test");
	std::cout << "Wc -l " << FsWc(f) << endl;
	std::cout << "Wc -l " << FsWc("/Users/r0ssar00/test") << endl;
	list<string> g = FsHead(f, 5);
	list<string> h = FsTail(f, 4);
	list<string> i = FsExecuteQuery("School/Year 1/ITI1100");
	list<string> l = FsExecuteQuery("XTagRevConfigFile");
	list<string>::iterator j;
	for (j=g.begin(); j !=g.end(); j++) {
		cout << "Line: " << *j << endl;
	}
	for (j=h.begin(); j !=h.end(); j++) {
		cout << "Line: " << *j << endl;
	}
	for (j=i.begin(); j !=i.end(); j++) {
		cout << "Result: " << *j << endl;
	}
	for (j=l.begin(); j !=l.end(); j++) {
		cout << "Result: " << *j << endl;
	}
	cout << "Count: " << i.size() << endl;
	pthread_t maint;
	char * mess = "t1";
	pthread_mutex_init(&TagsLock, NULL);
	InitMutex();
	pthread_create(&maint, NULL, doadd, (void*)mess);
	pthread_join(maint,NULL);

	// do stuff
	DestroyMutex();
	pthread_mutex_destroy(&TagsLock);*/
	cout << "DBus starting\n";
	// hey, lets try some dbus stuff
	cout << "setting up signals\n";
	signal(SIGTERM, niam);
	signal(SIGINT, niam);
	cout << "setting up dispatcher\n";
	DBus::default_dispatcher = &dispatcher;
	cout << "setting up connection\n";
	DBus::Connection conn = DBus::Connection::SessionBus();
	cout << "request name\n";
	conn.request_name("com.ross.XTagRev.TagDB");
	cout << "creating object\n";
	TagDBsrv server(conn);
	cout << "starting mainloop\n";
	dispatcher.enter();
    return 0;
};

#pragma mark -
#pragma mark -
#pragma mark -
#pragma mark Implementations
// Platform implementations
