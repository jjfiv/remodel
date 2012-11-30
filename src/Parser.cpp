#include "Parser.h"
#include "Token.h"
#include <deque>
#include <set>
#include <fstream>

static void parseError(const char *msg) {
  cerr << msg << '\n';
  exit(-1);
}

static void unexpectedToken(const Token &token) {
  cerr << "Unexpected Token("<<token<<")\n";
  exit(-1);
}


static Token readNextToken(std::istream &input) {
  // constants
  const string followSet = "<,: ";
  const std::set<string> accept = {Syntax::LeftArrow, Syntax::Colon, Syntax::Comma};
  
  // state
  string partial;
  bool inQuotes = false;

  while(input.good()) {
    int x = input.get();

    if(!inQuotes) {
      // convert all whitespace to ' '
      if(x >= 0 && x < ' ') x = ' ';

      // possible terminal characters for a token
      if(x == EOF || followSet.find(x) != string::npos) {
        // if we've collected something, yield it now
        if(partial.size() > 0) {
          input.putback(x);
          break;
        }
      }

      if(x == ' ') continue;
      if(x == EOF) return Syntax::EndOfFile;
    } else {
      if(x == EOF) parseError("EOF found during quoted literal");
    }

    partial += x;

    // if this is the terminating quote, we're done, otherwise change state
    if(x == '"') {
      if(inQuotes) break;
      inQuotes = true;
      continue;
    }

    // if this is a literal part of the grammar, be done here
    if(accept.find(partial) != accept.end())
      break;

    // single character token
    if(x == ':' || x == ',')
      break;
  }

  return Token(partial);
}

static BuildRule parseRule(std::deque<Token> &tokens) {
  BuildRule rule;

  // get targets
  while(1) {
    const Token target = tokens.front();
    if(!target.isNode())
      unexpectedToken(target);

    rule.addTarget(BuildNode(target.getString()));
    tokens.pop_front();

    if(!tokens.front().isComma()) {
      break;
    }
    // drop comma
    tokens.pop_front();
  }

  if(!tokens.front().isLeftArrow())
    unexpectedToken(tokens.front());
  tokens.pop_front();
  
  // get sources
  while(1) {
    const Token source = tokens.front();
    if(!source.isNode())
      unexpectedToken(source);

    rule.addSource(BuildNode(source.getString()));
    tokens.pop_front();

    if(!tokens.front().isComma()) {
      break;
    }
    tokens.pop_front();
  }

  if(tokens.front().isColon()) {
    tokens.pop_front();
    const Token action = tokens.front();
    if(!action.isAction())
      unexpectedToken(action);

    rule.action = BuildAction(action.getString());
    tokens.pop_front();
  }

  return rule;
}

vector<BuildRule> parseFile(const string &fileName) {
  cout << "parsing \""<<fileName<<"\"\n";

  std::ifstream fp(fileName);

  if(fp.bad()) {
    cerr << fileName << " could not be opened";
    exit(-1);
  }

  std::deque<Token> tokens;

  while(fp.good()) {
    Token t = readNextToken(fp);
    if(t.isEOF())
      break;
    tokens.push_back(t);
  }

  vector<BuildRule> rules;

  while(tokens.size()) {
    BuildRule rule = parseRule(tokens);
    if(!rule.valid()) {
      cerr << "Bad rule.\n";
      break;
    }
    
    rules.push_back(rule);
  }

  return rules;
}


  
