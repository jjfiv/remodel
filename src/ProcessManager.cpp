#include "ProcessManager.h"
#include "remodel.h" // for ShellCommand
#include <sys/types.h>
#include <sys/wait.h>

#define USE_SYSTEM 0

bool ProcessManager::spawn(const string &cmd, void *data) {
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

#if 1
  _exit(system(local.c_str()));
  return false;
#else
  const char* interpreter = remodel::ShellCommand;
  show(local.c_str());
  show(interpreter);
  // child continues on to exec
  int rc = execl(interpreter, interpreter, "-c", local.c_str());
  if(rc == -1) {
    perror("execl");
  }
  // should never get here
  _exit(-1);
  // should never never get here
  assert(0);
  return -1;
#endif
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
      cleanExit(-1);
    }
    return ProcessResult();
  }

  // grab user data pointer
  auto it = childData.find(whom);
  assert(it != childData.end());
  void *ptr = it->second;

  // erase this process entry
  childData.erase(it);

  return ProcessResult(ptr, status);
}


