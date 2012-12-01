#ifndef _TARGETBUILDER_H
#define _TARGETBUILDER_H

#include "BuildRecord.h"
#include "BuildGraph.h"
#include "ProcessManager.h"
#include "FileOps.h"

class TargetBuilder {
  public:
    TargetBuilder(const BuildGraph &graph, const string &target, int numJobs=-1);

    bool build();
    vector<BuildRecord> getBuildRecords() const;

  private:
    BuildRecord recordForTarget(const BuildStep* step) const;
    bool startTarget(const BuildStep *step);
    
    bool awaitChild(bool block);
    void collectReadyChildren(bool expectOne);
    bool hasChildren() const { return pm.numChildren() > 0; }

    string updateHash(const BuildStep *step);

    int maxChildren;
    ProcessManager pm;
    std::map<int, TargetState> targetStates;
    std::set<const BuildStep*> targetSteps;
};

#endif

