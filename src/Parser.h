#ifndef _PARSER_H
#define _PARSER_H

#include "Syntax.h"

class ParseRule {
  public:
    bool valid() const { return hasTargets() && hasSources(); }
    bool hasTargets() const { return targets.size() != 0; }
    bool hasSources() const { return sources.size() != 0; }
    bool hasAction() const { return action.size() != 0; }

    void addTarget(const string &b) { targets.push_back(b); }
    void addSource(const string &b) { sources.push_back(b); }

    std::ostream& print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream &out, const ParseRule &b) { return b.print(out); }

    vector<string> targets;
    vector<string> sources;
    string action;
};

//--- parse a file to a list of rules
vector<ParseRule> parseFile(const string &fileName);

#endif

