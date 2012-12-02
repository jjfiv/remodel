#include "TargetBuilder.h"

TargetBuilder::TargetBuilder(const BuildGraph &graph, const string &target, const vector<BuildRecord> &records, int numJobs) {
  maxChildren = numJobs;
  targetSteps = graph.getTargetAndDeps(target);

  for(auto *step : targetSteps) {
    targetStates[step->id] = TargetState("init");
    updateHash(step);
  }
}

string TargetBuilder::updateHash(const BuildStep *step) {
  string hash = fileSignature(step->name);
  targetStates[step->id] = hash;
  return hash;
}

BuildRecord TargetBuilder::recordForTarget(const BuildStep* step) const {
  return BuildRecord(step, targetStates);
}

bool TargetBuilder::startTarget(const BuildStep *step) {
  // no need to restart
  if(targetStates[step->id].started)
    return false;

  // if there's a limit on the number of jobs, respect it here
  while(maxChildren > 0 && pm.numChildren() >= (size_t) maxChildren) {
    awaitChild(true);
  }

  const string &cmd = step->action;
  // echo action we're about to take
  cout << cmd << '\n';
  if(!pm.spawn(cmd, step)) {
    cerr << "Spawning `" << cmd << "' failed!\n";
    exit(-1);
  }
  targetStates[step->id].started = true;

  return true;
}

bool TargetBuilder::targetDone(const BuildStep *step) const {
  if(!step->phony()) {
    const BuildRecord t = recordForTarget(step);
    for(const BuildRecord &r : prevRecords) {
      if(r == t) return true;
    }
  }

  auto it = targetStates.find(step->id);
  assert(it != targetStates.end());
  return it->second.built;
}

bool TargetBuilder::awaitChild(bool block) {
  auto res = pm.waitNextChild( block );
  if(!res.valid) {
    assert(!block);
    return false;
  }

  if(!res.success()) {
    cerr << "Command failed.\n";
    exit(-1);
  }

  const BuildStep *step = (const BuildStep*)res.data;
  targetStates[step->id].built = true;
  if(updateHash(step) == "") {
    cerr << "Action `" << step->action << "' failed to build target `" << step->name << "'";
    exit(-1);
  }

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
      if(step->isDone())
        continue;

      if(step->isSource()) {
        cerr << "Source file `" << step->name << "' could not be found.\n";
        exit(-1);
      }

      // not all done, this one isn't, at least
      allDone = false;

      if(!step->isReady())
        continue;

      if(step->hasAction()) {
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
    if(!anyReady) {
      cerr << "Not done building, but nothing ready to build :(\n";
      exit(-1);
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

