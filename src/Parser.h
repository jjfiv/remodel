#ifndef _PARSER_H
#define _PARSER_H

#include "Syntax.h"

class BuildAction {
  public:
    BuildAction(const string& cmd="") : command(cmd) { }
    bool nop() const { return command == ""; }

    std::ostream& print(std::ostream& out) const {
      return out << '"' << command << '"';
    }
    string get() const { return command; }
  private:
    string command;
};

class BuildNode {
  public:
    BuildNode(const string &fileName)
      : name(fileName) { };

    std::ostream& print(std::ostream &out) const {
      return out << name;
    }

    string name;
};

class BuildRule {
  public:
    bool valid() const { return hasTargets() && hasSources(); }
    bool hasTargets() const { return targets.size() != 0; }
    bool hasSources() const { return sources.size() != 0; }
    bool hasAction() const { return !action.nop(); }

    void addTarget(const BuildNode &b) { targets.push_back(b); }
    void addSource(const BuildNode &b) { sources.push_back(b); }

    std::ostream& print(std::ostream& out) const {
      for(size_t i=0; i<targets.size(); i++) {
        if(i != 0) out << Syntax::Comma << " ";
        targets[i].print(out);
      }

      out << " " << Syntax::LeftArrow << " ";
      
      for(size_t i=0; i<sources.size(); i++) {
        if(i != 0) out << Syntax::Comma << " ";
        sources[i].print(out);
      }

      if(!action.nop()) {
        out << Syntax::Colon << " ";
        action.print(out);
      }

      return out;
    }

    friend std::ostream& operator<<(std::ostream &out, const BuildRule &b) { b.print(out); return out; }


    vector<BuildNode> targets;
    vector<BuildNode> sources;
    BuildAction action;
};

//--- parse a file to a list of rules
vector<BuildRule> parseFile(const string &fileName);

#endif

