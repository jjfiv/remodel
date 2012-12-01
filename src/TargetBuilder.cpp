#include "TargetBuilder.h"

TargetBuilder::TargetBuilder(const BuildGraph &graph, const string &target, int numJobs) {
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

