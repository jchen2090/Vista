#include "token.h"

#include <iostream>

void Token::print() const {

  std::string typeName;

  switch (type) {
  case (TokenType::ASSIGN):
    typeName = "ASSIGN";
    break;
  case (TokenType::EOL_TOKEN):
    typeName = "EOL";
    break;
  case (TokenType::EOF_TOKEN):
    typeName = "EOF";
    break;
  case (TokenType::INT_KEYWORD):
    typeName = "INT_KEYWORD";
    break;
  case (TokenType::FLOAT_KEYWORD):
    typeName = "FLOAT_KEYWORD";
    break;
  case (TokenType::STR_KEYWORD):
    typeName = "STR_KEYWORD";
    break;
  case (TokenType::IDENTIFIER):
    typeName = "IDENTIFIER";
    break;
  case (TokenType::COUT_KEYWORD):
    typeName = "COUT";
    break;
  case (TokenType::INT):
    typeName = "INT";
    break;
  case (TokenType::FLOAT):
    typeName = "FLOAT";
    break;
  case (TokenType::STR):
    typeName = "STR";
    break;
  case (TokenType::OPEN_PARENTHESES):
    typeName = "OPEN PARENTHESES";
    break;
  case (TokenType::CLOSE_PARENTHESES):
    typeName = "CLOSE PARENTHESES";
    break;
  default:
    typeName = "DID NOT DEFINE TYPE";
    break;
  }

  std::cout << typeName << "(" << val << ")";
}
