#ifndef _TARGETBUILDER_H
#define _TARGETBUILDER_H

#include "BuildRecord.h"
#include "BuildGraph.h"
#include "ProcessManager.h"
#include "FileOps.h"

class TargetBuilder {
  public:
    TargetBuilder(const BuildGraph &graph, const string &target, const vector<BuildRecord> &records, int numJobs=-1);

    bool build();
    vector<BuildRecord> getBuildRecords() const;

  private:
    BuildRecord recordForTarget(const BuildStep* step) const;
    bool startTarget(const BuildStep *step);
    
    bool targetDone(const BuildStep *step) const;
    bool targetReady(const BuildStep *step) const;

    const ActionState& getActionState(const BuildStep *step) const {
      auto it = actionStates.find(step->action);
      assert(it != actionStates.end());
      return it->second;
    }
    ActionState& getActionState(const BuildStep *step) { 
      auto it = actionStates.find(step->action);
      assert(it != actionStates.end());
      return it->second;
    }
    
    bool awaitChild(bool block);
    void collectReadyChildren(bool expectOne);
    bool hasChildren() const { return pm.numChildren() > 0; }

    string updateHash(const BuildStep *step);

    int maxChildren;
    ProcessManager pm;
    std::map<string, ActionState> actionStates;
    std::map<int, string> targetStates;
    vector<BuildRecord>        prevRecords;
    std::set<const BuildStep*> targetSteps;
};

#endif

