#include "FileOps.h"
#include "ProcessManager.h"
#include "Parser.h"
#include "Arguments.h"
#include "remodel.h"
#include "BuildGraph.h"
#include <fstream>

#include "TargetBuilder.h"


bool buildTarget(const BuildGraph &buildSet, const string target) {
  TargetBuilder builder(buildSet, target);
  
  bool somethingChanged = builder.build();

  for(const BuildRecord &r : builder.getBuildRecords()) {
    cout << r << "\n";
  }

  return somethingChanged;
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
  const auto OPT_REMAKE = args.defFlag({"--remake"}, "Delete all generated files and make targets anyway.");
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

  if(args.getFlag(OPT_CLEAN) || args.getFlag(OPT_REMAKE)) {
    cleanTargets(buildSet);
    if(!args.getFlag(OPT_REMAKE)) exit(0);
  }

  for(auto t: currentTargets) {
    if(!buildTarget(buildSet, t)) {
      cout << "remodel: `" << t << "' is up to date.\n";
    }
  }

  return 0;
}

