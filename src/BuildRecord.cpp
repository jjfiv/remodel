#include "BuildRecord.h"

BuildRecord::BuildRecord(const BuildStep *step, std::map<int, TargetState> targetStates) {
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

BuildRecord::BuildRecord(std::istream &input) {
  // expect at least name and hash
  complete = (input >> name) && (input >> hash);

  if(!complete)
    return;
  
  while(input.good()) {
    string dn, dh;

    if(!(input >> dn) || !(input >> dh) )
      return;

    depHash[dn] = dh;
  }
}

bool BuildRecord::operator==(const BuildRecord &rhs) const {
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

bool BuildRecord::hasDependency(const string &fileName) const {
  assert(complete);
  return depHash.find(fileName) != depHash.end();
}

std::ostream& BuildRecord::print(std::ostream &out) const {
  if(!complete) {
    return out << "Incomplete BuildRecord: `" << name << "'";
  }

  out << name << " " << hash << " ";

  if(depHash.size()) {
    for(auto dep : depHash) {
      out << dep.first << " " << dep.second << " ";
    }
  }

  return out;
}


