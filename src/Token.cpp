#include "Token.h"
#include "Syntax.h"

namespace {
  const std::map<string, Token::Type> literals = {
    {Syntax::EndOfFile, Token::EndOfFile},
    {Syntax::Colon, Token::Colon},
    {Syntax::Comma, Token::Comma},
    {Syntax::LeftArrow, Token::LeftArrow},
  };
}

Token::Token(const string &input) : type(Error) {

  // if it exists in map above, yield it
  auto it = literals.find(input);
  if(it != literals.end()) {
    type = it->second;
    return;
  }

  if(input[0] == '"') {
    assert(input.back() == '"');
    // save the unquoted part
    type = Action;
    data = input.substr(1, input.size()-2);
    return;
  }

  type = Node;
  data = input;
}

std::ostream& Token::print(std::ostream &out) const {
  switch(type) {
    case Node:
      return out << data;
    case Comma:
      return out << Syntax::Comma;
    case LeftArrow:
      return out << Syntax::LeftArrow;
    case Colon:
      return out << Syntax::Colon;
    case Action:
      return out << '"' << data << '"';
    case EndOfFile:
      return out << "EOF";
    default:
      return out << "ERR";
  }
}
