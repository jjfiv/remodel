#ifndef _ARGUMENTS_H
#define _ARGUMENTS_H

class Arguments {
  public:
    Arguments(int argc, char *argv[]) {
      for(int i=1; i<argc; i++)
        parameters.push_back(argv[i]);
    }

    size_t count() const { return parameters.size(); }

    bool has(const string &str) const {
      for(const auto &p : parameters)
        if(p == str) return true;
      return false;
    }

    const string& getAfter(const string &toFind, const string &fallback) {
      for(size_t i=0; i<count()-1; i++) {
        if(parameters[i] == toFind)
          return parameters[i+1];
      }
      return fallback;
    }

    const string& operator[](size_t index) { return parameters[index]; }
  
  private:
    vector<string> parameters;
};


#endif

