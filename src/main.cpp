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

vector<string> collectTargets(const Arguments &args) {
  vector<string> results;

  for(size_t i=0; i<args.size(); i++) {
    if(args[i][0] == '-') {
      i++;
      continue;
    }
    results.push_back(args[i]);
  }

  if(results.size() == 0)
    results.push_back(Syntax::DefaultTarget);

  return results;
}


class BuildStep {
  
  private:
    vector<string> dependency;
};

int main(int argc, char *argv[]) {
  Arguments args;

  const auto OPT_FILE = args.defOption({"-f", "--file"}, "Read FILE as a remodelfile.");
  const auto OPT_DIR = args.defOption({"-C", "--directory"}, "Change to DIRECTORY before doing anything.");
  const auto OPT_HELP = args.defFlag({"--help", "-?", "-h"}, "Show this help.");
  
  args.parse(argc, argv);

  if(args.getFlag(OPT_HELP)) {
    args.showHelp(std::cout);
    return 0;
  }

  // collect all arguments
  string targetFile = args.getOption(OPT_FILE,"remodelfile");
  string currentDir = args.getOption(OPT_DIR, ".");
  vector<string> currentTargets = collectTargets(args);

  // change directory if necessary
  if(!changeDirectory(currentDir)) {
    return -1;
  }

  // error if input file can't be found
  if(!canOpenFile(targetFile)) {
    cerr << "Cannot open " << currentDir << "/" << targetFile << "!\n";
    return -1;
  }

  // create a set of defined targets
  std::set <string> definedTargets;
  const auto rules = parseFile(targetFile);
  for(auto r: rules) {
    for(BuildNode &t : r.targets) {
      const string &tn = t.name;
      if(definedTargets.find(tn) != definedTargets.end()) {
        cerr << "Multiple rules for target `"<<tn<<"'!\n";
        return -1;
      }
      definedTargets.insert(tn);
    }
  }

  // ensure that arguments given to build are defined
  for(auto t: currentTargets) {
    if(definedTargets.find(t) == definedTargets.end()) {
      cerr << "remodel: *** target `"<<t<<"' not defined.\n";
      return -1;
    }
  }

  cout << "Defined Targets:\n";
  for(auto t: definedTargets) {
    cout << "  " << t << "\n";
  }

#if 0 
  for(auto r: rules) {
    //cout << r << '\n';
  }
  cout << "\n";
#endif

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

