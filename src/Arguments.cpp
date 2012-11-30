#include "Arguments.h"

string Arguments::ArgDef::key() const {
  return names.front();
}

bool Arguments::ArgDef::matches(const string &arg) const {
  for(const auto &n : names)
    if(n == arg) return true;
  return false;
}

string Arguments::defFlag(vector<string> names, string description) {
  argDefs.push_back( ArgDef(names, description, false) );
  return argDefs.back().key();
}

string Arguments::defOption(vector<string> names, string description) {
  argDefs.push_back( ArgDef(names, description, true) );
  return argDefs.back().key();
}

void Arguments::parse(int argc, char *argv[]) {
  progName=argv[0];

  for(int i=1; i<argc; i++) {
    string arg(argv[i]);

    bool found = false;
    for(auto def : argDefs) {
      if(!def.matches(arg))
        continue;
      
      found = true;

      // it is a flag if not an option
      if(!def.option) {
        flags.insert(def.key());
        break;
      }

      // it is an option
      if(i+1 == argc) {
        cerr << "Expected argument after `" << arg << "'";
        showHelp(std::cerr);
        exit(-1);
      }

      options[def.key()] = argv[i+1];
      i++;
      break;
    }
    
    // if it didn't match a definition, accumulate it as a generic "parameter"
    if(!found) {
      parameters.push_back(arg);
    }
  }
  // done with args
}

void Arguments::showHelp(std::ostream &out) const {
  out << progName << " help:\n";
  for(auto &def : argDefs) {
    for(size_t i=0; i<def.names.size(); i++) {
      if(i != 0) out << ", ";
      out << def.names[i];
    }
    out << "\n" << "  * " << def.description << '\n';
  }
  out << "\n";
}

bool Arguments::getFlag(const string &str) const {
  return flags.find(str) != flags.end();
}

const string& Arguments::getOption(const string &toFind, const string &fallback) const {
  auto it = options.find(toFind);
  if(it == options.end())
    return fallback;

  return it->second;
}


