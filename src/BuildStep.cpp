#include "BuildStep.h"
#include "FileOps.h"

std::ostream& BuildStep::print(std::ostream &out) const {
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

bool BuildStep::dependsOn(int other) const {
  for(BuildStep *bs : deps) {
    if(bs->id == other || bs->dependsOn(other))
      return true;
  }
  return false;
}

