#include "FileOps.h"
#include "ProcessManager.h"


int main(int argc, char *argv[]) {
  cout << "remodel\n";

  string testFile = "SConstruct";
  cout << "sig("<<testFile<<") = " << fileSignature(testFile) << "\n";
  //Process pmd5("md5sum "+testFile);
  //pmd5.wait();
  ProcessManager pm;

  pid_t a = pm.spawn("md5sum "+testFile);
  pid_t b = pm.spawn("file "+testFile);
  while(pm.numChildren() != 0) { 
    ProcessResult pr = pm.waitNextChild();
    assert(pr.success());
    assert(pr.id == a || pr.id == b);
  }

  return 0;
}

