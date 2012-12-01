#ifndef _BUILDGRAPH_H
#define _BUILDGRAPH_H

#include "Parser.h"
#include "BuildStep.h"
#include <set>

class BuildGraph {
  public:
    BuildGraph(const vector<BuildRule> &rules);
    ~BuildGraph();
   
    const std::set<const BuildStep*> getTargetAndDeps(const string &name) const;
    const BuildStep* getTarget(const string &name) const;

    const vector<BuildStep*> getVector() const { return steps; }

    void putDotFile(std::ostream &out, const string &graphName) const;

  private:
    vector<BuildStep*> steps;
};

#endif

