#ifndef _FILEOPS_H
#define _FILEOPS_H

#include "common.h"

bool canOpenFile(const string &path);
string fileSignature(const string &path);

bool removeFile(const string &path);

bool changeDirectory(const string &path);
bool createDirectory(const string &path);

// lock files
bool createLockfile(const string &path);
bool myLockFile(const string &path);



#endif

