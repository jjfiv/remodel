#include "FileOps.h"

int main(int argc, char *argv[]) {
  cout << "remodel\n";
  string testFile = "/etc/hosts";
  cout << "sig("<<testFile<<") = " << fileSignature(testFile) << "\n";
  return 0;
}

