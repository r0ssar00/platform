/*
 *  platform.h
 *  XTagRev
 *
 *  Created by Kevin Ross on 20/01/09.
 *
 */
#ifndef _PLATFORM_H
#define _PLATFORM_H
string FsGetTag(string arg1); //arg1=tag, returns tag
int FsSetTag(string arg1, string arg2); //arg1=file, arg2=tag, returns 1 on fail, 0 otherwise
list<string> FsQuery(string arg1); //arg1=query, returns list of files matching query
list<string> FsExecuteQuery(string arg1); //arg1=tag, returns list of matching files
// end indexing system functions
// filesystem and file operations (they abstract lots of code away from the main stuff)
list<string> FsCat(string arg1); //arg1=file, returns list of lines in file
int FsWc(list<string> arg1); //arg1=contents of file, line by line, as list, returns number of lines
int FsWc(string arg1); //arg1=file, returns number of lines in file
list<string> FsHead(list<string> arg1, int c); //arg1=contents of file, c=number of lines, returns c number of lines from top of file
list<string> FsTail(list<string> arg1, int c); //arg1=contents of file, c=number of lines, returns c number of lines from bottom of file
int FsRm(string arg1); //arg1=file, returns 1 if fail, 0 if otherwise
int FsRmDir(string arg1); //arg1=folder, returns 1 if fail, 0 if otherwise
int removedirectoryrecursively(string dirname);
list<string> FsLs(string arg1); // arg1=folder, returns list of files/folders in arg1
int FsCd(string arg1); //arg1=folder, returns 1 if fail, 0 if otherwise
int FsMkDir(string arg1); //arg1=folder, returns 1 if fail, 0 if otherwise
int FsCp(string arg1, string arg2); //arg1=from file, arg2=to file, returns 1 if fail, 0 if otherwise
int FsCpDir(string arg1, string arg2); //arg1=from folder, arg2=to folder, returns 1 if fail, 0 if otherwise
int FsTouch(string arg1); //arg1=file, returns 1 if fail, 0 if otherwise
list<string> FsSort(list<string> arg1); //arg1=list of strings, returns sorted arg1
list<string> FsFind(string arg1); //arg1=folder, returns list of files/folders found under arg1(recursive)
list<string> FsFindFiles(string arg1); // arg1=folder, returns list of only files in folder recursively
list<string> FsFindFolders(string arg1); // arg1=folder, returns list of only folders in folder recursively
int FsLn(string arg1, string arg2, string kind); //arg1=from, arg2=to, kind=link,alias(if possible), returns 1 if fail, 0 if otherwise
void FsEcho(string arg1); //arg1=text to print to screen
int FsEcho(string arg1, string arg2, bool append); //arg1=text to send to the end of file(arg2) of append, otherwise, erase file(arg2) and put arg1 into file(arg2)
int FsEcho(list<string> arg1, string arg2, bool append); //echo list of strings to file(arg2) with/without appending
#endif