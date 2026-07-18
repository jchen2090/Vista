#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include "../token/token.h"
#include <memory>
#include <vector>

class Parser {
private:
  std::vector<Token> tokens;
  unsigned int pos = 0;

  // Helper functions
  bool isAtEOF();
  Token peek();
  Token advance();
  Token consume(TokenType type, std::string msg);

  // Parsing functions
  std::unique_ptr<ASTNode> parseAssignmentStatement();
  std::unique_ptr<ExprNode> parseExpression();
  std::unique_ptr<ASTNode> parseCoutStatement();

public:
  Parser(std::vector<Token> listOfTokens);
  std::unique_ptr<RootNode> parse();
};

#endif
