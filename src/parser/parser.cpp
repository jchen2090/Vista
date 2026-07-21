#include "parser.h"
#include "../token/token.h"
#include <memory>
#include <stdexcept>

Parser::Parser(std::vector<Token> listOfTokens) { tokens = listOfTokens; }

// Returns the current token
Token Parser::peek() {
  if (pos >= tokens.size()) {
    return Token{TokenType::EOF_TOKEN, ""};
  }
  return tokens[pos];
}

// Returns current token and advances
Token Parser::advance() {
  if (pos >= tokens.size()) {
    return Token{TokenType::EOF_TOKEN, ""};
  }
  return tokens[pos++];
}

// Checks to see if we've finished going through all the tokens
bool Parser::isAtEOF() { return peek().type == TokenType::EOF_TOKEN; }

Token Parser::consume(TokenType expectedType, std::string errMsg) {
  Token token = peek();

  if (token.type != expectedType) {
    throw std::runtime_error("Syntax Error: " + errMsg + " (Found: '" +
                             token.val + "')");
  }
  return advance();
}

// Parses expressions such as binary operations and whatnot, anything that
// results in a numerical/string value
std::unique_ptr<ExprNode> Parser::parseExpression() {
  if (peek().type == TokenType::INT) {
    Token t = advance();
    int value = std::stoi(t.val);
    return std::make_unique<IntNode>(value);
  }

  if (peek().type == TokenType::FLOAT) {
    Token t = advance();
    double value = std::stod(t.val);
    return std::make_unique<FloatNode>(value);
  }

  if (peek().type == TokenType::STR) {
    Token t = advance();
    return std::make_unique<StrNode>(t.val);
  }

  if (peek().type == TokenType::IDENTIFIER) {
    Token t = advance();
    return std::make_unique<IdentifierNode>(t.val);
  }

  throw std::runtime_error("Syntax Error: Expected a valid literal value.\n");
  exit(1);
}

// Parse variable initialization
std::unique_ptr<ASTNode> Parser::parseAssignmentStatement() {
  Token type = advance();

  Token id =
      consume(TokenType::IDENTIFIER, "Expected a variable name after type.");
  consume(TokenType::ASSIGN, "Expected '=' after variable name.");

  std::unique_ptr<ExprNode> value = parseExpression();

  consume(TokenType::EOL_TOKEN, "Expected new line after declaration.");

  return std::make_unique<AssignmentStatement>(type.val, id.val,
                                               std::move(value));
}

std::unique_ptr<ASTNode> Parser::parseCoutStatement() {
  advance();

  consume(TokenType::OPEN_PARENTHESES, "Expected '('");
  std::unique_ptr<ExprNode> value = parseExpression();
  consume(TokenType::CLOSE_PARENTHESES, "Expected ')'");
  consume(TokenType::EOL_TOKEN, "Expected new line after print statement.");

  return std::make_unique<PrintStatement>(std::move(value));
}

std::unique_ptr<RootNode> Parser::parse() {
  std::unique_ptr<RootNode> root = std::make_unique<RootNode>();

  while (!isAtEOF()) {
    Token token = peek();

    // Skip new lines
    if (token.type == TokenType::EOL_TOKEN) {
      advance();
      continue;
    }

    // Parse assignments (e.g int x = 5)
    if (token.type == TokenType::FLOAT_KEYWORD ||
        token.type == TokenType::INT_KEYWORD ||
        token.type == TokenType::STR_KEYWORD) {
      root->statements.push_back(parseAssignmentStatement());
      continue;
    }

    // Parse cout statements
    if (token.type == TokenType::COUT_KEYWORD) {
      root->statements.push_back(parseCoutStatement());
    }
  }
  return root;
}
