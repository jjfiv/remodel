#ifndef _TOKEN_H
#define _TOKEN_H

#include "Syntax.h"
#include <map>

class Token {
  public:
    enum Type {
      Error,
      Node,
      Comma,
      LeftArrow,
      Colon,
      Action,
      EndOfFile,
    };

    bool valid() const { return type != Error; }

    bool isNode() const { return type == Node; }
    bool isComma() const { return type == Comma; }
    bool isLeftArrow() const { return type == LeftArrow; }
    bool isColon() const { return type == Colon; }
    bool isAction() const { return type == Action; }
    bool isEOF() const { return type == EndOfFile; }

    Token(const string &input);

    // ostream operator
    std::ostream& print(std::ostream &out) const;
    friend std::ostream& operator<<(std::ostream& out, const Token &t) { return t.print(out); }

    string getString() const { return data; }
  
    Type type;
  private:
    string data;
};

#endif

