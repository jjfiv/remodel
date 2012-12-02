#include "BuildGraph.h"
#include "remodel.h"

class BuildIDTable {
  public:
    void put(const string &node) { 
      if(ids.find(node) == ids.end()) {
        size_t id = ids.size();
        ids[node] = id;
      }
    }
    int get(const string &node) {
      return ids[node];
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

BuildGraph::BuildGraph(const vector<ParseRule> &rules) {
  BuildIDTable bs;
  std::set<string> uniqTargets;

  for(auto r: rules) {
    for(string &t : r.targets) {
      if(uniqTargets.find(t) != uniqTargets.end()) {
        startErr() << "Multiple definition for target `" << t << "'\n";
        cleanExit(-1);
      }
      uniqTargets.insert(t);
      bs.put(t);
    }
    for(string &s : r.sources) { bs.put(s); }
  }

  steps.resize(bs.size());
  for(auto &it : bs.all()) {
    int id = it.second;
    const string &name = it.first;
    steps[id] = new BuildStep(id, name);
  }

  // insert dependencies based on steps
  for(auto r: rules) {
    for(string &t : r.targets) {
      BuildStep *cur = steps[bs.get(t)];

      for(string &src : r.sources) {
        BuildStep *dep = steps[bs.get(src)];
        cur->deps.push_back(dep);
      }

      cur->action = r.action;
    }
  }

  // check for circular dependencies and for rules that need actions
  bool error = false;
  for(auto *s : steps) {
    if(s->isCircular()) {
      startErr() << "Circular dependency detected in target: `" << s->name << "'\n";
      error = true;
    }
    // if it's not a fake target, and not a pure source, it should have an action
    if(!s->phony() && !s->isSource() && !s->hasAction()) {
      auto &err = startErr() << "No action to make target: `" << s->name << "' from ";
      for(size_t i=0; i<s->deps.size(); i++) {
        if(i != 0) err << ", ";
        err << s->deps[i]->name;
      }
      err << "\n";
      error = true;
    }
  }

  if(error) {
    cleanExit(-1);
  }

}

BuildGraph::~BuildGraph() {
  for(BuildStep *step : steps) {
    delete step;
  }
}

const std::set<const BuildStep*> BuildGraph::getTargetAndDeps(const string &targetName) const {
  std::set<const BuildStep*> result;

  // initial set
  const BuildStep *first = getTarget(targetName);
  assert(first != nullptr);
  result.insert(first);

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

const BuildStep* BuildGraph::getTarget(const string &name) const {
  for(auto bs : steps) {
    if(bs->name == name) return bs;
  }
  return nullptr;
}

void BuildGraph::putDotFile(std::ostream &out, const string &graphName) const {
  out << "digraph " << graphName << " {";
  for(const BuildStep *s : steps) {
    out << "  step" << s->id << "\n";
    out << "  step" << s->id << "[label=\"" << s->name << "\"]\n";

    for(auto *d : s->deps) {
      out << "  step" << s->id << " -> step" << d->id << "\n";
    }

  }
  out << "}\n";
}

