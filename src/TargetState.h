#ifndef _TARGETSTATE_H
#define _TARGETSTATE_H

#include "common.h"
class BuildStep;

// mutable ActionState object; from not existing to started to built
struct ActionState {
  public:
    // create and accumulate targets that are relevant
    ActionState() : state(INIT) { }

    // state machine
    void start() { assert(state == INIT); state = STARTED; }
    void markDone() { assert(state == STARTED); state = DONE; }
    
    bool hasStarted() const { return state != INIT; }
    bool isDone() const { return state == DONE; }

    vector<const BuildStep *> targets;
  private:
    enum State {
      INIT,
      STARTED,
      DONE,
      ERROR,
    };
    State state;
};

#endif

