#include "FileOps.h"
#include "ProcessManager.h"
#include "Parser.h"
#include "Arguments.h"
#include "remodel.h"
#include "BuildGraph.h"
#include <fstream>

bool buildTarget(const BuildGraph &buildSet, const string target) {
  ProcessManager pm;

  int actions = 0;
  std::set<const BuildStep*> targetSet = buildSet.getTargetAndDeps(target);

  while(1) {
    bool allDone = true;
    bool anyReady = false;

    for(auto *step : targetSet) {
      show(*step);
      if(step->isDone())
        continue;

      // not all done, this one isn't, at least
      allDone = false;

      if(!step->isReady())
        continue;

      if(step->hasAction()) {
        // not all blocked, this one is ready, so start it
        anyReady = true;
        actions++;

        // echo action we're about to take
        cout << step->action << '\n';
        if(pm.spawn(step->action) < 0) {
          cerr << "Spawning `" << step->action << "' failed!\n";
          exit(-1);
        }
      }
    }

    // terminate when finished building this target
    if(allDone)
      break;

    // if we can't make forward progress, catch this condition
    if(!anyReady) {
      cerr << "Not done building, but nothing ready to build :(\n";
      exit(-1);
    }

    // collect all processes we started this time about
    while(pm.numChildren() != 0) {
      auto res = pm.waitNextChild();
      if(!res.success()) {
        cerr << "Command failed.\n";
        exit(-1);
      }
    }
  }

  return actions != 0;
}

void cleanTargets(const BuildGraph &buildSet) {
  cout << "remodel: clean\n";
  for(auto *bs : buildSet.getVector()) {
    if(bs->shouldClean()) {
      if(removeFile(bs->name)) {
        cout << bs->name << " ";
      }
    }
  }
  cout << "\n";
}

int main(int argc, char *argv[]) {
  Arguments args;

  // hooray for C++11 initializer lists :D
  const auto OPT_FILE = args.defOption({"-f", "--file"}, "Read FILE as a remodelfile.");
  const auto OPT_DIR = args.defOption({"-C", "--directory"}, "Change to DIRECTORY before doing anything.");
  const auto OPT_GRAPH = args.defOption({"--graph"}, "Output a Graphviz dot file to FILE");
  const auto OPT_CLEAN = args.defFlag({"--clean"}, "Delete all generated files.");
  const auto OPT_HELP = args.defFlag({"--help", "-?", "-h"}, "Show this help.");
  
  args.parse(argc, argv);
  
  if(args.getFlag(OPT_HELP)) {
    args.showHelp(std::cout);
    return 0;
  }


  // collect all arguments
  string targetFile = args.getOption(OPT_FILE,"remodelfile");
  string currentDir = args.getOption(OPT_DIR, ".");
  vector<string> currentTargets = args.getParameters();
  if(currentTargets.size() == 0)
    currentTargets.push_back(Syntax::DefaultTarget);

  // change directory if necessary
  if(!changeDirectory(currentDir)) {
    return -1;
  }

  // error if input file can't be found
  if(!canOpenFile(targetFile)) {
    cerr << "Cannot open " << currentDir << "/" << targetFile << "!\n";
    return -1;
  }

  if(!createDirectory(remodel::InfoDir)) {
    cerr << "Cannot create info directory: `" << remodel::InfoDir << "'\n";
    return -1;
  }


  BuildGraph buildSet(parseFile(targetFile));

  // ensure that arguments given to build are defined
  for(auto t: currentTargets) {
    if(buildSet.getTarget(t) == nullptr) {
      cerr << "remodel: *** target `"<<t<<"' not defined.\n";
      return -1;
    }
  }

  string dotFile = args.getOption(OPT_GRAPH, "");
  if(dotFile.size() != 0) {
    std::ofstream fp(dotFile);
    buildSet.putDotFile(fp, "remodel");
    
    cout << "created dot file `"<<dotFile<<"'\n";
    return 0;
  }

  if(args.getFlag(OPT_CLEAN)) {
    cleanTargets(buildSet);
  }

  for(auto t: currentTargets) {
    if(!buildTarget(buildSet, t)) {
      cout << "remodel: `" << t << "' is up to date.\n";
    }
  }

  return 0;
}

