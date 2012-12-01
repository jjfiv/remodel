#ifndef _BUILDRECORD_H
#define _BUILDRECORD_H

#include "BuildStep.h"
#include "TargetState.h"
#include <map>

// defines the hashes of all the dependencies of a file when it was built
class BuildRecord {
  public:
    BuildRecord(const string &n, const string &h)
      : complete(false), name(n), hash(h) { }

    BuildRecord(const BuildStep *step, std::map<int, TargetState> targetStates) {
      complete = false;
      
      name = step->name;
      const TargetState &fs = targetStates[step->id];
      if(!fs.fileExists()) return;
      hash = fs.hash;

      for(auto *d : step->deps) {
        const TargetState &dfs = targetStates[d->id];
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
        return out << "Incomplete BuildRecord: `" << name << "'";
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


#endif

