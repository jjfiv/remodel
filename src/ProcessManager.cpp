#include "ProcessManager.h"
#include "remodel.h" // for ShellCommand
#include <sys/types.h>
#include <sys/wait.h>


bool ProcessManager::spawn(const string &cmd, const void *data) {
  pid_t child = -1;

  string local(cmd);

  child = vfork();
  
  // print error and go into error state
  if(child == -1) {
    perror("vfork");
    return false;
  }

  // parent receives child pid
  if(child != 0) {
    // assert that this pid_t is not already in the set...
    assert(childData.find(child) == childData.end());
    childData[child] = data;
    return true;
  }

  const char* interpreter = remodel::ShellCommand;
  // child continues on to exec
  execl(interpreter, interpreter, "-c", local.c_str());
  perror("execl");
  // should never get here
  _exit(-1);
  // should never never get here
  assert(0);
  return -1;
}

ProcessResult ProcessManager::waitNextChild(bool block) {
  int status;
  pid_t whom = -1;
  if(block) {
    whom = wait(&status);
  } else {
    whom = waitpid(-1, &status, WNOHANG);
  }
  if(whom <= 0) {
    if(whom < 0 || block) {
      perror("wait");
      exit(-1);
    }
    return ProcessResult();
  }

  // grab user data pointer
  auto it = childData.find(whom);
  assert(it != childData.end());
  const void *ptr = it->second;

  // erase this process entry
  childData.erase(it);

  return ProcessResult(ptr, status);
}


