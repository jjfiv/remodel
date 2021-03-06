#include "Parser.h"
#include "Token.h"
#include "remodel.h"
#include <deque>

static void unexpectedToken(const Token &token) {
  startErr() << "Unexpected Token("<<token<<")\n";
  cleanExit(-1);
}

std::ostream& ParseRule::print(std::ostream &out) const {
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
};

static Token readNextToken(std::istream &input) {
  // constants
  const string followSet = "#<,: ";
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

      if(x == Syntax::HashComment) {
        while(1) {
          x = input.get();
          if(x == '\n')
            break;
          if(x == EOF)
            return Syntax::EndOfFile;
        }
        continue;
      }
      if(x == ' ') continue;
      if(x == EOF) return Syntax::EndOfFile;
    } else {
      if(x == EOF) {
        startErr() << "EOF found during quoted literal";
        cleanExit(-1);
      }
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

static ParseRule parseRule(std::deque<Token> &tokens) {
  ParseRule rule;

  // get targets
  while(1) {
    const Token target = tokens.front();
    if(!target.isNode())
      unexpectedToken(target);

    rule.addTarget(target.getString());
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

    rule.addSource(source.getString());
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

    rule.action = action.getString();
    tokens.pop_front();
  }

  return rule;
}

vector<ParseRule> parseFile(const string &fileName) {
  //startMsg() << "parsing \""<<fileName<<"\"\n";

  std::ifstream fp(fileName);

  if(fp.bad()) {
    startErr() << "file `" << fileName << "' could not be opened\n";
    cleanExit(-1);
  }

  std::deque<Token> tokens;

  while(fp.good()) {
    Token t = readNextToken(fp);
    if(t.isEOF())
      break;
    tokens.push_back(t);
  }

  vector<ParseRule> rules;

  while(tokens.size()) {
    ParseRule rule = parseRule(tokens);
    if(!rule.valid()) {
      startErr() << "Bad rule.\n";
      break;
    }
    
    rules.push_back(rule);
  }

  return rules;
}


  
