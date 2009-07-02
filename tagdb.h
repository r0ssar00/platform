/*
 *  tagdb.h
 *  XTagRev
 *
 *  Created by Kevin Ross on 20/01/09.
 *
 */
#ifndef _TAGDB_H
#define _TAGDB_H
struct TagStruct {
	string name;
	int count;
}; // so I can get the tag count at the same time
void CreateTagDB(string arg1); //arg1=path to create tagdb at
int LoadTagDB(string arg1); //arg1=path to create tagdb at if it doesn't exist
void AddTagDB(string arg1);
void RmTagDB(string arg1);
vector<TagStruct> GetTagDB();
void InitMutex();
void DestroyMutex();
// thread stuff.  Mutex sets this
#endif