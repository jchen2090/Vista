#ifndef LEXER_H
#define LEXER_H

#include "../token/token.h"
#include <string>
#include <vector>

class Lexer {
private:
  std::string line;
  std::vector<Token> tokens;
  unsigned int pos = 0;

  char peek();
  char advance();
  Token nextToken();

public:
  Lexer(std::string sourceLine);
  std::vector<Token> tokenize();
};

#endif
