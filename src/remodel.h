#ifndef _REMODEL_H
#define _REMODEL_H

#include "common.h"

namespace remodel {
  const string InfoDir = ".remodel";
  const string LockFile = InfoDir + "/lock";
  const string RecordFile = InfoDir + "/records.txt";
  const char* const ShellCommand = "/bin/sh";

};

std::ostream& startMsg();
std::ostream& startErr();
void cleanExit(int status);

#endif

