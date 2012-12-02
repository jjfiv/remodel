#ifndef _BUILDSTEP_H
#define _BUILDSTEP_H

#include "Syntax.h"

class BuildStep {
  public:
    BuildStep(int i, string n) : id(i), name(n) { }
    bool phony() const { return name == Syntax::DefaultTarget; }
    bool hasAction() const { return action.size() != 0; }
    bool isSource() const { return !hasDependencies() && !hasAction(); }
    bool hasDependencies() const { return deps.size() != 0; }
    bool shouldClean() const { return !isSource() && !phony(); }

    // circular if it depends eventually on itself
    bool isCircular() const { return dependsOn(id); }
    bool dependsOn(int other) const;

    // output
    std::ostream& print(std::ostream &out) const;
    friend std::ostream& operator<<(std::ostream &out, const BuildStep &self) { return self.print(out); }

    int id;
    string name;
    string action;
    vector<BuildStep*> deps;

};


#endif

