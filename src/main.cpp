#include "remodel.h"
#include "FileOps.h"
#include "Parser.h"
#include "Arguments.h"
#include "BuildGraph.h"

#include "TargetBuilder.h"

// this is a helper function called anywhere error messages are needed
// defined in "remodel.h"
std::ostream& startErr() {
  return std::cerr << "remodel: *** ";
}

// this is a helper function called anywhere messages are needed
std::ostream& startMsg() {
  return std::cout << "remodel: ";
}

// cleans up lockfile if any before exiting
void cleanExit(int status) {
  if(myLockFile(remodel::LockFile))
    removeFile(remodel::LockFile);
  exit(status);
}

vector<BuildRecord> readBuildRecords() {
  vector<BuildRecord> inputRecords;

  std::ifstream fp(remodel::RecordFile);
  
  while(fp.good()) {
    string line;
    getline(fp, line);

    std::stringstream ss(line);

    BuildRecord r(ss);
    if(r.complete) {
      //show(r);
      inputRecords.push_back(r);
    }
  }

  return inputRecords;
}

void saveBuildRecords(const vector<BuildRecord> &input) {
  std::ofstream fp(remodel::RecordFile);
  for(const BuildRecord &r : input) {
    fp << r << "\n";
  }
}

bool buildTarget(const BuildGraph &buildSet, const string target) {
  TargetBuilder builder(buildSet, target, readBuildRecords());
  
  bool somethingChanged = builder.build();
  saveBuildRecords(builder.getBuildRecords());

  return somethingChanged;
}

void cleanTargets(const BuildGraph &buildSet) {
  auto &out = startMsg() << "clean\n";
  for(auto *bs : buildSet.getVector()) {
    if(bs->shouldClean()) {
      if(removeFile(bs->name)) {
        out << bs->name << " ";
      }
    }
  }
  out << "\n";
}

int main(int argc, char *argv[]) {
  Arguments args;

  // hooray for C++11 initializer lists :D
  const auto OPT_FILE = args.defOption({"-f", "--file"}, "Read FILE as a remodelfile.");
  const auto OPT_DIR = args.defOption({"-C", "--directory"}, "Change to DIRECTORY before doing anything.");
  

  const auto OPT_GRAPH = args.defOption({"--graph"}, "Output a Graphviz dot file to FILE");

  const auto OPT_WAIT = args.defFlag({"-w", "--wait"}, "Wait for lock if there is contention.");

  const auto OPT_CLEAN = args.defFlag({"--clean"}, "Delete all generated files.");
  const auto OPT_REMAKE = args.defFlag({"--remake"}, "Delete all generated files and make targets anyway.");
  const auto OPT_HELP = args.defFlag({"--help", "-?", "-h"}, "Show this help.");
  
  args.parse(argc, argv);
  
  if(args.getFlag(OPT_HELP)) {
    args.showHelp(startMsg());
    cleanExit(0);
  }


  // collect all arguments
  string targetFile = args.getOption(OPT_FILE,"remodelfile");
  string currentDir = args.getOption(OPT_DIR, ".");
  vector<string> currentTargets = args.getParameters();
  if(currentTargets.size() == 0)
    currentTargets.push_back(Syntax::DefaultTarget);

  // change directory if necessary
  if(!changeDirectory(currentDir)) {
    cleanExit(-1);
  }

  // error if input file can't be found
  if(!canOpenFile(targetFile)) {
    startErr() << "Cannot open " << currentDir << "/" << targetFile << "!\n";
    cleanExit(-1);
  }

  if(!createDirectory(remodel::InfoDir)) {
    startErr() << "Cannot create info directory: `" << remodel::InfoDir << "'\n";
    cleanExit(-1);
  }

  if(args.getFlag(OPT_WAIT)) {
    while(!createLockfile(remodel::LockFile)) {
      std::cout << "p" << getpid() <<  " spin...\n";
    }
  } else {
    if(!createLockfile(remodel::LockFile)) {
      startErr()
        << "Cannot create lockfile\n"
        << "  re-run with " << OPT_WAIT << " to wait for the lock\n"
        << "  If you are sure that no other instance of remodel is running,\n"
        << "  please delete `" << remodel::LockFile << "'\n\n";
      cleanExit(-1);
    }
  }


  BuildGraph buildSet(parseFile(targetFile));

  // ensure that arguments given to build are defined
  for(auto t: currentTargets) {
    if(buildSet.getTarget(t) == nullptr) {
      startErr() << "target `"<<t<<"' not defined.\n";
      cleanExit(-1);
    }
  }

  string dotFile = args.getOption(OPT_GRAPH, "");
  if(dotFile.size() != 0) {
    std::ofstream fp(dotFile);
    buildSet.putDotFile(fp, "remodel");
    fp.close();

    startMsg() << "created dot file `"<<dotFile<<"'\n";
    cleanExit(0);
  }

  if(args.getFlag(OPT_CLEAN) || args.getFlag(OPT_REMAKE)) {
    cleanTargets(buildSet);
    if(!args.getFlag(OPT_REMAKE))
      cleanExit(0);
  }

  for(auto t: currentTargets) {
    if(!buildTarget(buildSet, t)) {
      startMsg() << "target `" << t << "' is up to date.\n";
    }
  }

  cleanExit(0);
}

