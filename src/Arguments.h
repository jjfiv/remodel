#ifndef _ARGUMENTS_H
#define _ARGUMENTS_H

#include "common.h"
#include <set>
#include <map>

class Arguments {
  public:
    struct ArgDef {
      ArgDef(const vector<string> &n, string d, bool o)
        : names(n), description(d), option(o) { }
      string key() const { return names.front(); }
      bool matches(const string &arg) {
        for(const auto &n : names)
          if(n == arg) return true;
        return false;
      }
      vector<string> names;
      string description;
      bool option;
    };
  public:

    size_t size() const { return parameters.size(); }

    // setup
    string defFlag(vector<string> names, string description);
    string defOption(vector<string> names, string description);

    // parse
    void parse(int argc, char *argv[]);

    // help
    void showHelp(std::ostream &out) const;

    // query
    bool getFlag(const string &str) const;
    const string& getOption(const string &toFind, const string &fallback) const;

    const string& operator[](size_t index) const { return parameters[index]; }

  
  private:
    string progName;
    vector<ArgDef> argDefs;

    std::set<string> flags;
    std::map<string, string> options;
    vector<string> parameters;
};


#endif

