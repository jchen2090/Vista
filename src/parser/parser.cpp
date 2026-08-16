#include "parser.h"
#include "../token/token.h"
#include "ast/ast.h"
#include "type/types.h"
#include <memory>
#include <stdexcept>

// Maps TokenType Enum to Type Enum
Type tokenToType(TokenType tokenType) {
  if (tokenType == TokenType::FLOAT_KEYWORD) {
    return Type::FLOAT;
  } else if (tokenType == TokenType::INT_KEYWORD) {
    return Type::INT;
  } else if (tokenType == TokenType::STR_KEYWORD) {
    return Type::STR;
  }
  return Type::UNKNOWN;
}

BinaryOp tokenToBinOp(TokenType tokenType) {
  if (tokenType == TokenType::ADD) {
    return BinaryOp::ADD;
  } else if (tokenType == TokenType::SUBTRACT) {
    return BinaryOp::SUBTRACT;
  } else if (tokenType == TokenType::MULTIPLY) {
    return BinaryOp::MULTIPLY;
  } else if (tokenType == TokenType::DIVIDE) {
    return BinaryOp::DIVIDE;
  }
  throw std::runtime_error("Unable to convert token to binary op");
}

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

// Parses the actual data type,
std::unique_ptr<ExprNode> Parser::parseValue() {
  if (peek().type == TokenType::INT) {
    Token t = consume(TokenType::INT, "Expected int literal");
    int value = std::stoi(t.val);
    return std::make_unique<IntNode>(value);
  }

  if (peek().type == TokenType::FLOAT) {
    Token t = consume(TokenType::FLOAT, "Expected float literal");
    double value = std::stod(t.val);
    return std::make_unique<FloatNode>(value);
  }

  if (peek().type == TokenType::STR) {
    Token t = consume(TokenType::STR, "Expected str literal");
    return std::make_unique<StrNode>(t.val);
  }

  if (peek().type == TokenType::IDENTIFIER) {
    Token t = consume(TokenType::IDENTIFIER, "Expected identifier");
    return std::make_unique<IdentifierNode>(t.val);
  }

  throw std::runtime_error("Syntax Error: Expected a valid literal value.\n");
}

// Parses expressions such as binary operations and whatnot, anything that
// results in a numerical/string value
std::unique_ptr<ExprNode> Parser::parseExpression() {
  std::unique_ptr<ExprNode> lhs = parseValue();

  while (peek().type == TokenType::ADD || peek().type == TokenType::SUBTRACT ||
         peek().type == TokenType::MULTIPLY ||
         peek().type == TokenType::DIVIDE) {
    Token op = advance();
    std::unique_ptr<ExprNode> rhs = parseValue();

    lhs = std::make_unique<BinaryOpNode>(std::move(lhs), tokenToBinOp(op.type),
                                         std::move(rhs));
  }
  return lhs;
}

// Parse variable initialization
std::unique_ptr<ASTNode> Parser::parseAssignmentStatement() {
  Type type = tokenToType(advance().type);

  Token id =
      consume(TokenType::IDENTIFIER, "Expected a variable name after type.");
  consume(TokenType::ASSIGN, "Expected '=' after variable name.");

  std::unique_ptr<ExprNode> value = parseExpression();

  consume(TokenType::EOL_TOKEN, "Expected new line after declaration.");

  return std::make_unique<AssignmentStatement>(type, id.val, std::move(value));
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
      continue;
    }

    // Parse binary expresseions
    if (token.type == TokenType::INT || token.type == TokenType::FLOAT ||
        token.type == TokenType::IDENTIFIER) {
      root->statements.push_back(parseExpression());
      continue;
    }

    std::string msg = "Unable to parse token \n";
    throw std::runtime_error(msg);
  }
  return root;
}
