#ifndef _ARGUMENTS_H
#define _ARGUMENTS_H

class Arguments {
  public:
    Arguments(int argc, char *argv[]) {
      for(int i=1; i<argc; i++)
        parameters.push_back(argv[i]);
    }

    size_t size() const { return parameters.size(); }

    bool has(const string &str) const {
      for(const string &p : parameters) {
        if(p == str) return true;
      }
      return false;
    }

    const string& hasAfter(const string &toFind, const string &fallback) const {
      for(size_t i=0; i<size()-1; i++) {
        if(parameters[i] == toFind) {
          return parameters[i+1];
        }
      }
      return fallback;
    }

    const string& operator[](size_t index) const { return parameters[index]; }

    friend std::ostream& operator<<(std::ostream& out, const Arguments &args) {
      out << "[";
      for(size_t i=0; i<args.size(); i++) {
        if(i != 0) out << ", ";
        out << args[i];
      }
      return out << "]";
    }
  
  private:
    vector<string> parameters;
};


#endif

