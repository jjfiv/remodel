#ifndef _BUILDRECORD_H
#define _BUILDRECORD_H

#include "BuildStep.h"
#include "TargetState.h"

// defines the hashes of all the dependencies of a file when it was built
class BuildRecord {
  public:
    BuildRecord(const string &n, const string &h)
      : complete(false), name(n), hash(h) { }
    BuildRecord(const BuildStep *step, std::map<int, TargetState> targetStates);
    BuildRecord(std::istream &input);
    
    bool operator==(const BuildRecord &rhs) const;
    bool hasDependency(const string &fileName) const;

    std::ostream& print(std::ostream &out) const;
    friend std::ostream& operator<<(std::ostream &out, const BuildRecord &b) { return b.print(out); }

    bool complete;
  private:
    string name;
    string hash;
    std::map<string, string> depHash;
};


#endif

