#ifndef _PROCESSMANAGER_H
#define _PROCESSMANAGER_H

#include <unistd.h> // for pid_t
#include "common.h"

struct ProcessResult {
  ProcessResult() : valid(false), data(nullptr), status(-1) { }
  ProcessResult(void *ptr, int ret) : valid(true), data(ptr), status(ret) { }
  bool success() const { return valid && status == 0; }
  
  const bool valid;
  void *data;
  int status;
};

class ProcessManager {
  public:
    // non-blocking call to create a child
    bool spawn(const string &cmd, void *data);
    // blocking call to wait for the next child to finish
    ProcessResult waitNextChild(bool shouldBlock=true);
    // return the number of children left
    size_t numChildren() const { return childData.size(); }
  private:
    std::map<pid_t, void*> childData;
};

#endif

