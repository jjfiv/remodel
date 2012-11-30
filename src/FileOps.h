#ifndef _FILEOPS_H
#define _FILEOPS_H

#include "common.h"

string fileSignature(const string &path);
bool changeDirectory(const string &path);
bool canOpenFile(const string &path);


#endif

