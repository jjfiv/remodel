#include "Process.h"

#include <sys/types.h>
#include <sys/wait.h>

Process::Process(const string &cmd) {

  // put this object in an error state by default
  child = -1;

  string local(cmd);

  // vfork is a special brand of fork that is only good for exec'ing after
  pid_t result = vfork();

  // print error and go into error state
  if(result == -1) {
    perror("vfork");
    return;
  }

  // parent receives child pid
  if(result != 0) {
    child = result;
    return;
  }

  // child continues on to exec
  execl("/bin/sh", "sh", "-c", local.c_str());
  perror("execl");
  _exit(-1);
}

int Process::wait() {
  int status = 0;
  if(waitpid(child, &status, 0) == -1) {
    perror("waitpid");
    return -1;
  }
  return status;
}

