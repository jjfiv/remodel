#include "FileOps.h"
#include "ProcessManager.h"
#include "Parser.h"
#include "Arguments.h"
#include "remodel.h"

class BuildIDTable {
  public:
    void put(const BuildNode &node) { 
      const string &name = node.name;
      if(ids.find(name) == ids.end()) {
        size_t id = ids.size();
        ids[name] = id;
      }
    }
    int get(const BuildNode &node) {
      return ids[node.name];
    }
    void print(std::ostream &out) const {
      for(auto it : ids) {
        out << "  " <<  it.second << ". " << it.first << "\n";
      }
    }
    size_t size() const { return ids.size(); }
    std::map<string, int> all() const { return ids; }
  private:
    std::map<string, int> ids;
};

class BuildStep {
  public:
    BuildStep(int i, const string &n) : id(i), name(n) { }

    bool phony() const { return name == Syntax::DefaultTarget; }
    bool hasAction() const { return action.size() != 0; }
    bool isSource() const { return !hasDependencies() && !hasAction(); }
    bool hasDependencies() const { return deps.size() != 0; }
    bool shouldClean() const { return !isSource() && !phony(); }

    std::ostream& print(std::ostream &out) const {
      out << name;
      
      if(hasDependencies()) {
        out << " : {";
        for(size_t i=0; i<deps.size(); i++) {
          if(i != 0) out << ",";
          out << deps[i]->id;
        }
        out << "}";
      }

      if(hasAction()) {
        out << " $ " << action;
      }
      
      return out;
    }

    friend std::ostream& operator<<(std::ostream &out, const BuildStep &self) { return self.print(out); }

    int id;
    string name;
    string action;
    vector<BuildStep*> deps;

    bool depsOn(int other) const {
      for(BuildStep *bs : deps) {
        if(bs->id == other || bs->depsOn(other))
          return true;
      }
      return false;
    }

    bool isReady() const {
      for(auto *bs : deps) {
        if(!bs->isDone()) return false;
      }
      return true;
    }

    // mutable state :(
    bool isDone() const {
      if(!phony()) {
        // semi-gmake version
        return canOpenFile(name);
      }
      return isReady(); // equal logically to "are my dependencies done?"
    }
};

class BuildSet {
  public:
    BuildSet(const vector<BuildRule> &rules) {
      BuildIDTable bs;

      for(auto r: rules) {
        for(BuildNode &t : r.targets) { bs.put(t); }
        for(BuildNode &s : r.sources) { bs.put(s); }
      }

      steps.resize(bs.size());
      for(auto &it : bs.all()) {
        int id = it.second;
        const string &name = it.first;
        steps[id] = new BuildStep(id, name);
      }

      // insert dependencies based on steps
      for(auto r: rules) {
        for(BuildNode &t : r.targets) {
          BuildStep *cur = steps[bs.get(t)];

          for(BuildNode &src : r.sources) {
            BuildStep *dep = steps[bs.get(src)];
            cur->deps.push_back(dep);
          }

          cur->action = r.action.get();
        }
      }

    };

    ~BuildSet() {
      for(const BuildStep *bs : steps) {
        assert(bs);
        delete bs;
      }
    }
   
    const std::set<const BuildStep*> getTargetAndDeps(const string &name) const {
      std::set<const BuildStep*> result;
      
      // initial set
      result.insert(getTarget(name));

      while(1) {
        std::set<const BuildStep*> newSet;
        
        for(auto *t : result) {
          for(auto *d : t->deps) {
            if(result.find(d) != result.end())
              continue;
            newSet.insert(d);
          }
        }

        if(!newSet.size())
          break;

        for(auto *t : newSet)
          result.insert(t);
      }

      return result;
    }

    const BuildStep* getTarget(const string &name) const {
      for(auto *bs : steps) {
        if(bs->name == name) return bs;
      }
      return nullptr;
    }
    const BuildStep* getTarget(int id) const { return steps[id]; }
    const vector<BuildStep*>& getVector() const { return steps; }

  private:
    vector<BuildStep*> steps;
};

bool buildTarget(const BuildSet &buildSet, const string target) {
  ProcessManager pm;

  int actions = 0;
  std::set<const BuildStep*> targetSet = buildSet.getTargetAndDeps(target);

  while(1) {
    bool allDone = true;
    bool anyReady = false;
    for(auto *step : targetSet) {
      if(step->isDone())
        continue;

      // not all done, this one isn't, at least
      allDone = false;

      if(!step->isReady())
        continue;

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

void cleanTargets(const BuildSet &buildSet) {
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


  BuildSet buildSet(parseFile(targetFile));

  // ensure that arguments given to build are defined
  for(auto t: currentTargets) {
    if(buildSet.getTarget(t) == nullptr) {
      cerr << "remodel: *** target `"<<t<<"' not defined.\n";
      return -1;
    }
  }

  if(args.getFlag(OPT_CLEAN)) {
    cleanTargets(buildSet);
    return 0;
  }

  for(auto t: currentTargets) {
    if(!buildTarget(buildSet, t)) {
      cout << "remodel: `" << t << "' is up to date.\n";
    }
  }

  return 0;
}

