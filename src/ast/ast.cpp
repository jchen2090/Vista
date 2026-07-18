#include "ast.h"
#include <iostream>
#include <string>

void AssignmentStatement::debugPrint(int indent) const {
  std::cout << std::string(indent, ' ') << "Assignment: " << typeAnnotaiton
            << " " << identifier << " = \n";

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

void IntNode::debugPrint(int indent) const {
  std::cout << std::string(indent, ' ') << "IntLiteral: " << value << "\n";
}

void FloatNode::debugPrint(int indent) const {
  std::cout << std::string(indent, ' ') << "FloatLiteral: " << value << "\n";
}

void StrNode::debugPrint(int indent) const {
  // Wrapping in quotes makes strings much easier to visually debug
  std::cout << std::string(indent, ' ') << "StrLiteral: \"" << value << "\"\n";
}
