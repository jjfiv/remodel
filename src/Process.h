#ifndef _PROCESS_H
#define _PROCESS_H

#include <unistd.h>
#include "common.h"

class Process {
  public:
    Process(const string &cmd);

    int wait();
    
  private:
    pid_t child;
};

#endif

