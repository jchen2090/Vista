#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
  ASSIGN,
  EOL_TOKEN,
  EOF_TOKEN,
  INT_KEYWORD,
  STR_KEYWORD,
  FLOAT_KEYWORD,
  IDENTIFIER,
  COUT_KEYWORD,
  FLOAT,
  INT,
  STR,
  OPEN_PARENTHESES,
  CLOSE_PARENTHESES,
  ADD,
  SUBTRACT,
  MULTIPLY,
  DIVIDE
};

struct Token {
  TokenType type;
  std::string val;

  void print() const;
};

#endif
