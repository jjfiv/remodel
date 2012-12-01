#include "BuildGraph.h"
#include <map>

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

BuildGraph::BuildGraph(const vector<BuildRule> &rules) {
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

