#include "ast.h"
#include "type/types.h"
#include <iostream>
#include <string>

// Change from Type Enum to String for debugging/logging purposes
std::string enumToString(Type type) {
  if (type == Type::INT) {
    return "int";
  } else if (type == Type::FLOAT) {
    return "float";
  } else if (type == Type::STR) {
    return "str";
  }
  return "UNKNOWN";
}

void AssignmentStatement::debugPrint(int indent) const {
  std::cout << std::string(indent, ' ')
            << "Assignment: " << enumToString(typeAnnotation) << " "
            << identifier << " = \n";

  if (value) {
    value->debugPrint(indent + 2);
  } else {
    std::cout << std::string(indent + 2, ' ') << "[nullptr]\n";
  }
}

void PrintStatement::debugPrint(int indent) const {
  std::cout << std::string(indent, ' ') << "Cout: " << "\n";

  if (value) {
    value->debugPrint(indent + 2);
  } else {
    std::cout << std::string(indent + 2, ' ') << "[nullptr]\n";
  }
}

void RootNode::debugPrint(int indent) const {
  std::cout << "Program AST:\n";
  for (const auto &stmt : statements) {
    stmt->debugPrint(indent + 2);
  }
}

void IdentifierNode::debugPrint(int indent) const {
  std::cout << std::string(indent, ' ') << "IdentifierNode: " << identifier
            << "\n";
}

Type IdentifierNode::getExpression() const { return Type::IDENTIFIER; }

void IntNode::debugPrint(int indent) const {
  std::cout << std::string(indent, ' ') << "IntLiteral: " << value << "\n";
}

Type IntNode::getExpression() const { return Type::INT; }

void FloatNode::debugPrint(int indent) const {
  std::cout << std::string(indent, ' ') << "FloatLiteral: " << value << "\n";
}

Type FloatNode::getExpression() const { return Type::FLOAT; }

void StrNode::debugPrint(int indent) const {
  // Wrapping in quotes makes strings much easier to visually debug
  std::cout << std::string(indent, ' ') << "StrLiteral: \"" << value << "\"\n";
}

Type StrNode::getExpression() const { return Type::STR; }
