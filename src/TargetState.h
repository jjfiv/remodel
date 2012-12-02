#ifndef _TARGETSTATE_H
#define _TARGETSTATE_H

#include "common.h"

// mutable TargetState object; from not existing to started to built
struct TargetState {
  public:
    TargetState(const string &h="") : started(false), built(false), hash(h) { }
    bool fileExists() const { return hash.size() != 0; }

    void markDone() { started = true; built = true; }

    bool started;
    bool built;
    string hash;
};

#endif

