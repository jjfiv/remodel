#include "FileOps.h"
#include "Process.h"

int main(int argc, char *argv[]) {
  cout << "remodel\n";
  
  string testFile = "SConstruct";
  cout << "sig("<<testFile<<") = " << fileSignature(testFile) << "\n";
  Process pmd5("md5sum "+testFile);
  pmd5.wait();

  return 0;
}

