#include "ProcessManager.h"
#include <sys/types.h>
#include <sys/wait.h>


pid_t ProcessManager::spawn(const string &cmd) {
  pid_t child = -1;

  string local(cmd);

  child = fork();
  
  // print error and go into error state
  if(child == -1) {
    perror("vfork");
    return -1;
  }

  // parent receives child pid
  if(child != 0) {
    // assert that this pid_t is not already in the set...
    assert(children.find(child) == children.end());
    children.insert(child);
    return child;
  }

  const char* interpreter = "/bin/sh";
  // child continues on to exec
  execl(interpreter, interpreter, "-c", local.c_str());
  perror("execl");
  // should never get here
  _exit(-1);
  // should never never get here
  assert(0);
  return -1;
}

ProcessResult ProcessManager::waitNextChild() {
  int status;
  pid_t whom = wait(&status);
  if(whom < 0) {
    perror("wait");
    return ProcessResult(-1, -1);
  }

  auto it = children.find(whom);
  assert(it != children.end());

  children.erase(it);

  return ProcessResult(whom, status);
}


