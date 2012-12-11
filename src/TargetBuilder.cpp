#include "TargetBuilder.h"
#include "remodel.h"

TargetBuilder::TargetBuilder(const BuildGraph &graph, const string &target, const vector<BuildRecord> &records, int numJobs) {
  maxChildren = numJobs;
  targetSteps = graph.getTargetAndDeps(target);
  prevRecords = records;

  for(auto *step : targetSteps) {
    // create an appropriate actionState
    if(step->hasAction()) {
      auto it = actionStates.find(step->action);
      auto &action = (it == actionStates.end()) ? actionStates[step->action] : it->second;
      if(!step->phony()) {
        action.targets.push_back(step);
      }
    }

    if(!step->phony()) {
      // create an appropriate targetState (hash)
      targetStates[step->id] = "";
      updateHash(step);
    }
  }
}

string TargetBuilder::updateHash(const BuildStep *step) {
  if(step->phony()) return "";

  string hash = fileSignature(step->name);
  targetStates[step->id] = hash;
  return hash;
}

BuildRecord TargetBuilder::recordForTarget(const BuildStep* step) const {
  return BuildRecord(step, targetStates);
}

bool TargetBuilder::startTarget(const BuildStep *step) {
  const string &cmd = step->action;
  ActionState &action = getActionState(step);
  
  // no need to restart
  if(action.hasStarted())
    return false;

  // if there's a limit on the number of jobs, respect it here
  while(maxChildren > 0 && pm.numChildren() >= (size_t) maxChildren) {
    awaitChild(true);
  }

  // echo action we're about to take
  std::cout << cmd << '\n';
  if(!pm.spawn(cmd, &action)) {
    startErr() << "spawning `" << cmd << "' failed!\n";
    cleanExit(-1);
  }
  action.start();

  return true;
}

bool TargetBuilder::targetDone(const BuildStep *step) const {
  if(step->isSource())
    return canOpenFile(step->name);

  if(!step->phony()) {
    const BuildRecord t = recordForTarget(step);
    for(const BuildRecord &r : prevRecords) {
      if(r == t) return true;
    }
  }

  // phony targets that don't have actions are done by default; they're just used to find the closure of targets to build
  if(step->phony() && !step->hasAction())
      return true;


  return getActionState(step).isDone();
}

bool TargetBuilder::targetReady(const BuildStep *step) const {
  for(auto *d : step->deps) {
    if(!targetDone(d))
      return false;
  }
  return true;
}

bool TargetBuilder::awaitChild(bool block) {
  auto res = pm.waitNextChild( block );
  if(!res.valid) {
    assert(!block);
    return false;
  }

  if(!res.success()) {
    startErr() << "Command failed.\n";
    cleanExit(-1);
  }

  ActionState *action = (ActionState*)res.data;

  for(const BuildStep *step : action->targets) {
    if(updateHash(step) == "") {
      startErr() << "Action `" << step->action << "' failed to build target `" << step->name << "'";
      cleanExit(-1);
    }
  }
  action->markDone();

  return true;
}

void TargetBuilder::collectReadyChildren(bool expectOne) {
  bool block = expectOne;

  while(pm.numChildren() > 0) {
    if(!awaitChild(block))
      break; // didn't expect, didn't find

    block = false;
  }
}

vector<BuildRecord> TargetBuilder::getBuildRecords() const {
  vector<BuildRecord> results;
  results.reserve(targetSteps.size());

  for(auto *s : targetSteps) {
    if(!s->phony()) {
      results.push_back(recordForTarget(s));
    }
  }

  return results;
}

bool TargetBuilder::build() {
  int actions = 0;

  while(1) {
    bool allDone = true;
    bool anyReady = false;
    bool anyNew = false;

    for(auto *step : targetSteps) {
      if(targetDone(step))
        continue;

      if(step->isSource()) {
        startErr() << "Source file `" << step->name << "' could not be found.\n";
        cleanExit(-1);
      }

      // not all done, this one isn't, at least
      allDone = false;

      if(!targetReady(step))
        continue;
      
      if(step->hasAction() && !getActionState(step).hasStarted()) {
        // not all blocked, this one is ready, so start it
        anyReady = true;
        actions++;

        startTarget(step);
      }
    }

    // terminate when finished building this target
    if(allDone)
      break;

    // if we can't make forward progress, catch this condition
    if(!anyReady && pm.numChildren() == 0) {
      startErr() << "Not done building, but nothing ready to build :(\n";
      cleanExit(-1);
    }

    // block if there aren't any new processes this time, no point in going around again
    bool block = !anyNew;
    collectReadyChildren(block);
  }

  while(hasChildren()) {
    awaitChild(true);
  }

  return actions != 0;
}

