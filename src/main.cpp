#include "FileOps.h"
#include "ProcessManager.h"
#include "Parser.h"
#include "Arguments.h"

void printHelp() {
  cout << "remodel help\n"
          "  -f FILE      Read FILE as a remodelfile\n"
          "  -C DIRECTORY Change to DIRECTORY before doing anything\n"
          "  -h | --help  Show this help.\n"
          "\n";
}

int main(int argc, char *argv[]) {
  Arguments args(argc, argv);

  if(args.has("--help") || args.has("-h") || args.has("-?")) {
    printHelp();
    return 0;
  }

  string targetFile = args.getAfter("-f","remodelfile");
  string currentDir = args.getAfter("-C", ".");

  if(!changeDirectory(currentDir)) {
    return -1;
  }

  const auto rules = parseFile(targetFile);
  for(auto r: rules) {
    cout << r << '\n';
  }
  cout << "\n";

#if 0
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
#endif

  return 0;
}

