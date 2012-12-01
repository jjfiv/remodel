#ifndef _PARSER_H
#define _PARSER_H

#include "Syntax.h"

class BuildRule {
  public:
    bool valid() const { return hasTargets() && hasSources(); }
    bool hasTargets() const { return targets.size() != 0; }
    bool hasSources() const { return sources.size() != 0; }
    bool hasAction() const { return action.size() != 0; }

    void addTarget(const string &b) { targets.push_back(b); }
    void addSource(const string &b) { sources.push_back(b); }

    std::ostream& print(std::ostream& out) const {
      for(size_t i=0; i<targets.size(); i++) {
        if(i != 0) out << Syntax::Comma << " ";
        out << targets[i];
      }

      if(hasSources()) {
        out << " " << Syntax::LeftArrow << " ";

        for(size_t i=0; i<sources.size(); i++) {
          if(i != 0) out << Syntax::Comma << " ";
          out << sources[i];
        }
      }

      if(hasAction()) {
        out << Syntax::Colon << " " << action;
      }

      return out;
    }

    friend std::ostream& operator<<(std::ostream &out, const BuildRule &b) { b.print(out); return out; }


    vector<string> targets;
    vector<string> sources;
    string action;
};

//--- parse a file to a list of rules
vector<BuildRule> parseFile(const string &fileName);

#endif

