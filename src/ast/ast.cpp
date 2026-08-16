#include "ast.h"
#include "type/types.h"
#include <iostream>
#include <stdexcept>
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
Type IdentifierNode::getType() const { return Type::IDENTIFIER; }

void IntNode::debugPrint(int indent) const {
  std::cout << std::string(indent, ' ') << "IntLiteral: " << value << "\n";
}
Type IntNode::getType() const { return Type::INT; }

void FloatNode::debugPrint(int indent) const {
  std::cout << std::string(indent, ' ') << "FloatLiteral: " << value << "\n";
}
Type FloatNode::getType() const { return Type::FLOAT; }

void StrNode::debugPrint(int indent) const {
  // Wrapping in quotes makes strings much easier to visually debug
  std::cout << std::string(indent, ' ') << "StrLiteral: \"" << value << "\"\n";
}
Type StrNode::getType() const { return Type::STR; }

void BinaryOpNode::debugPrint(int indent) const {
  std::string operatorStr;

  switch (op) {
  case BinaryOp::ADD:
    operatorStr = "+";
    break;
  case BinaryOp::SUBTRACT:
    operatorStr = "-";
    break;
  case BinaryOp::MULTIPLY:
    operatorStr = "*";
    break;
  case BinaryOp::DIVIDE:
    operatorStr = "/";
    break;
  default:
    throw std::runtime_error("Invalid binary expression operator");
  };
  std::cout << std::string(indent, ' ') << "BinaryExprNode: " << operatorStr
            << "\n";

  if (lhs) {
    lhs->debugPrint(indent + 2);
  }
  if (rhs) {
    rhs->debugPrint(indent + 2);
  }
}
Type BinaryOpNode::getType() const { return Type::BIN_OP; }
