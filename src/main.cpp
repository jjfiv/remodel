#include "FileOps.h"
#include "ProcessManager.h"
#include "Parser.h"


int main(int argc, char *argv[]) {
  cout << "remodel\n";

  if(argc != 2) {
    cerr << "remodel currently takes a file name as input...\n";
    return -1;
  }

  const auto rules = parseFile(argv[1]);
  for(auto r: rules) {
    cout << r << '\n';
  }
  cout << "\n";

  string testFile = "SConstruct";
  cout << fileSignature(testFile) << "  " << testFile << "\n";
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

