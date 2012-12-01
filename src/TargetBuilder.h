#ifndef _TARGETBUILDER_H
#define _TARGETBUILDER_H

#include "BuildRecord.h"
#include "BuildGraph.h"
#include "ProcessManager.h"
#include "FileOps.h"

class TargetBuilder {
  public:
    TargetBuilder(const BuildGraph &graph, const string &target, int numJobs=-1);

    BuildRecord recordForTarget(const BuildStep* step) const;

    bool startTarget(const BuildStep *step);
    bool awaitChild(bool block);
    void collectReadyChildren(bool expectOne);

    bool hasChildren() const { return pm.numChildren() > 0; }

    std::set<const BuildStep*> targetSteps;
  private:
    string updateHash(const BuildStep *step);

    int maxChildren;
    ProcessManager pm;
    std::map<int, TargetState> targetStates;
};

#endif

