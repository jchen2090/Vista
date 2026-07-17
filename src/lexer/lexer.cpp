#include "lexer.h"
#include "../token/token.h"
#include <cctype>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

Lexer::Lexer(std::string sourceLine) { line = sourceLine; }

// Returns the current character
char Lexer::peek() {
  if (pos >= line.length()) {
    return '\0';
  }
  return line[pos];
}

// Returns the current character then goes to the next
char Lexer::advance() {
  if (pos >= line.length()) {
    return '\0';
  }
  return line[pos++];
}

// Returns the next token
Token Lexer::nextToken() {
  while (peek() != '\0') {
    char currentChar = peek();

    // Skip white spaces
    if (std::isspace(currentChar)) {
      if (currentChar == '\n') {
        advance();
        return Token{TokenType::EOL_TOKEN, ""};
      }
      advance();
      continue;
    }

    // Check for opening parentheses
    if (currentChar == '(') {
      advance();
      return Token{TokenType::OPEN_PARENTHESES, "("};
    }

    // Check for closing parentheses
    if (currentChar == ')') {
      advance();
      return Token{TokenType::CLOSE_PARENTHESES, ")"};
    }

    // Check for assignment
    if (currentChar == '=') {
      advance();
      return Token{TokenType::ASSIGN, "="};
    }

    // Check for string values
    if (currentChar == '"') {
      std::string currentStr = "";
      advance();

      while (peek() != '"' && peek() != '\0') {
        currentStr += advance();
      }

      if (peek() == '\0') {
        std::cerr << "Lexical Error: Invalid string literal '" << currentStr
                  << "' is missing end \"" << std::endl;
        exit(1);
      }
      advance();

      return Token{TokenType::STR, currentStr};
    }

    // Check for keywords/indentifiers
    if (std::isalpha(currentChar)) {
      std::string currentKeyword = "";

      while (std::isalnum(peek())) {
        currentKeyword += advance();
      }

      if (currentKeyword == "int") {
        return Token{TokenType::INT_KEYWORD, currentKeyword};
      }

      if (currentKeyword == "float") {
        return Token{TokenType::FLOAT_KEYWORD, currentKeyword};
      }

      if (currentKeyword == "str") {
        return Token{TokenType::STR_KEYWORD, currentKeyword};
      }

      if (currentKeyword == "cout") {
        return Token{TokenType::COUT_KEYWORD, currentKeyword};
      }

      return Token{TokenType::IDENTIFIER, currentKeyword};
    }

    // Process numbers/floats
    if (std::isdigit(currentChar)) {
      std::string currentNumber = "";
      bool isFloat = false;

      while (std::isdigit(peek()) || peek() == '.') {
        if (peek() == '.') {
          // If we experience multiple decimal points e.g 3..14 we do not parse
          if (isFloat) {
            std::cerr << "Lexical Error: Invalid numeric literal '"
                      << currentNumber << peek() << "' has too many decimals"
                      << std::endl;
            exit(1);
          }
          isFloat = true;
        }

        currentNumber += advance();
      }

      if (isFloat) {
        return Token{TokenType::FLOAT, currentNumber};
      }
      return Token{TokenType::INT, currentNumber};
    }

    std::cerr << "Can't recognize current character " << currentChar
              << std::endl;
    advance();
  }
  return Token{TokenType::EOF_TOKEN, ""};
}

std::vector<Token> Lexer::tokenize() {
  Token token = nextToken();
  tokens.push_back(token);

  while (token.type != TokenType::EOF_TOKEN) {
    token = nextToken();
    tokens.push_back(token);
  }

  return tokens;
}
