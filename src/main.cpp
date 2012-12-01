#include "FileOps.h"
#include "ProcessManager.h"
#include "Parser.h"
#include "Arguments.h"
#include "remodel.h"
#include "BuildGraph.h"
#include <fstream>

// mutable FileState object; from not existing to started to built
struct FileState {
  public:
    FileState(const string &h="") : started(false), built(false), hash(h) { }
    bool fileExists() const { return hash.size() != 0; }

    bool started;
    bool built;
    string hash;
};

// defines the hashes of all the dependencies of a file when it was built
class BuildRecord {
  public:
    BuildRecord(const string &n, const string &h)
      : complete(false), name(n), hash(h) { }

    BuildRecord(const BuildStep *step, std::map<int, FileState> targetStates) {
      complete = false;
      
      name = step->name;
      const FileState &fs = targetStates[step->id];
      if(!fs.fileExists()) return;
      hash = fs.hash;

      for(auto *d : step->deps) {
        const FileState &dfs = targetStates[d->id];
        if(!dfs.fileExists()) return;
        depHash[d->name] = dfs.hash;
      }
      
      complete = true;
    }
    
    bool operator==(const BuildRecord &rhs) {
      if(!complete || !rhs.complete) return false;
      if(name != rhs.name) return false;
      if(hash != rhs.hash) return false;

      if(depHash.size() != rhs.depHash.size())
        return false;

      for(auto mine : depHash) {
        auto other = rhs.depHash.find(mine.first);
        if(other==rhs.depHash.end() || other->second != mine.second)
          return false;
      }

      return true;
    }

    bool hasDependency(const string &fileName) {
      assert(complete);
      return depHash.find(fileName) != depHash.end();
    }


    std::ostream& print(std::ostream &out) const {
      if(!complete) {
        return out << "Incomplete BuildRecord";
      }


      out << name << ":" << hash;
      
      if(depHash.size()) {
        out << " <- {";
        for(auto dep : depHash) {
          out << dep.first << ":" << dep.second << " ";
        }
        out << "}";
      }

      return out;
    }

    friend std::ostream& operator<<(std::ostream &out, const BuildRecord &b) { return b.print(out); }

    bool complete;
  private:
    string name;
    string hash;
    std::map<string, string> depHash;
};


bool buildTarget(const BuildGraph &buildSet, const string target) {
  ProcessManager pm;

  int actions = 0;
  std::set<const BuildStep*> targetSet = buildSet.getTargetAndDeps(target);
  std::map<int, FileState> targetStates;

  // load up the states of the current files
  for(auto *step : targetSet) {
    targetStates[step->id] = FileState(fileSignature(step->name));
  }

  while(1) {
    bool allDone = true;
    bool anyReady = false;
    bool anyNew = false;

    for(auto *step : targetSet) {
      BuildRecord r(step, targetStates);
      show(r);

      if(step->isDone())
        continue;

      if(step->isSource()) {
        cerr << "Source file `" << step->name << "' could not be found.\n";
        exit(-1);
      }

      // not all done, this one isn't, at least
      allDone = false;

      if(!step->isReady())
        continue;

      if(step->hasAction()) {
        // not all blocked, this one is ready, so start it
        anyReady = true;
        actions++;

        // only run 1 process at a time for this target
        if(targetStates[step->id].started)
          continue;

        anyNew = true;

        const string &cmd = step->action;
        // echo action we're about to take
        cout << cmd << '\n';

        if(!pm.spawn(cmd, step)) {
          cerr << "Spawning `" << cmd << "' failed!\n";
          exit(-1);
        }
        targetStates[step->id].started = true;
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

    // block if there aren't any new processes this time, no point in going around again
    bool block = !anyNew;

    // collect all processes we started this time about
    while(pm.numChildren() != 0) {

      auto res = pm.waitNextChild( block );
      if(!res.valid) {
        break;
      }

      if(!res.success()) {
        cerr << "Command failed.\n";
        exit(-1);
      }


      const BuildStep *step = (const BuildStep*)res.data;
      targetStates[step->id].built = true;
      string hash = fileSignature(step->name);
      targetStates[step->id].hash = hash;
      if(hash == "") {
        cerr << "Action `" << step->action << "' failed to build target `" << step->name << "'";
        exit(-1);
      }

      // if go around and check for any other finished processes
      if(block) block = false;
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

