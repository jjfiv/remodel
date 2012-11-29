#ifndef _PROCESSMANAGER_H
#define _PROCESSMANAGER_H

#include <set>
#include <unistd.h> // for pid_t
#include "common.h"

struct ProcessResult {
  ProcessResult(pid_t pid, int ret) : id(pid), status(ret) { }
  bool success() const { return status == 0; }
  pid_t id;
  int status;
};

class ProcessManager {
  public:
    // non-blocking call to create a child
    pid_t spawn(const string &cmd);
    // blocking call to wait for the next child to finish
    ProcessResult waitNextChild();
    // return the number of children left
    size_t numChildren() const { return children.size(); }
  private:
    std::set<pid_t> children;
};

#endif

