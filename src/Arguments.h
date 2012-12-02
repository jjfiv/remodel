#ifndef _ARGUMENTS_H
#define _ARGUMENTS_H

#include "common.h"

class Arguments {
  public:
    struct ArgDef {
      // constructor
      ArgDef(const vector<string> &n, string d, bool o)
        : names(n), description(d), option(o) { }
      
      // manipulate
      string key() const;
      bool matches(const string &arg) const;

      // fields
      vector<string> names;
      string description;
      bool option;
    };
  public:

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
    const vector<string>& getParameters() const { return parameters; }

  
  private:
    string progName;
    vector<ArgDef> argDefs;

    std::set<string> flags;
    std::map<string, string> options;
    vector<string> parameters;
};


#endif

