#include "typeChecker.h"
#include <iostream>
#include <memory>
#include <stdexcept>

TypeChecker::TypeChecker(std::unique_ptr<RootNode> ast) {
  root = std::move(ast);
}

Type TypeChecker::parseExpression(std::unique_ptr<ExprNode> node) {
  if (dynamic_cast<IntNode *>(node.get())) {
    return Type::INT;
  } else if (dynamic_cast<StrNode *>(node.get())) {
    return Type::STR;
  } else if (dynamic_cast<FloatNode *>(node.get())) {
    return Type::FLOAT;
  } else if (auto *varNode = dynamic_cast<IdentifierNode *>(node.get())) {
    if (symbolTable.find(varNode->identifier) == symbolTable.end()) {
      return Type::UNKNOWN;
    }
    return symbolTable[varNode->identifier];
  }
  return Type::UNKNOWN;
}

Type TypeChecker::stringToType(std::string str) {
  if (str == "int") {
    return Type::INT;
  } else if (str == "float") {
    return Type::FLOAT;
  } else if (str == "str") {
    return Type::STR;
  }
  return Type::UNKNOWN;
}

// This is only used to handle error printing.
// We should try to use enums as much as we can.
std::string TypeChecker::typeToString(Type t) {
  switch (t) {
  case Type::INT:
    return "int";
  case Type::FLOAT:
    return "float";
  case Type::STR:
    return "str";
  case Type::UNKNOWN:
    return "unknown";
  default:
    throw std::runtime_error("Unknown enum type provided");
  }
}

void TypeChecker::validate() {
  for (std::unique_ptr<ASTNode> &node : root->statements) {

    // Checks typing for variable declaration and initialization
    if (AssignmentStatement *assignmentNode =
            dynamic_cast<AssignmentStatement *>(node.get())) {

      Type lhs = stringToType(assignmentNode->typeAnnotaiton);
      Type rhs = parseExpression(std::move(assignmentNode->value));

      if (rhs == Type::UNKNOWN || lhs != rhs) {
        std::string msg = "TypeError: Cannot assign '" + typeToString(rhs) +
                          "' to variable '" + assignmentNode->identifier +
                          "' of type " + typeToString(lhs) +
                          " with a value of type '" + typeToString(rhs) + "'";
        throw std::runtime_error(msg);
      }
      symbolTable[assignmentNode->identifier] = rhs;
    }
    // Handles type checking for print statements
    else if (PrintStatement *printNode =
                 dynamic_cast<PrintStatement *>(node.get())) {

      Type val = parseExpression(std::move(printNode->value));

      if (val == Type::UNKNOWN) {
        throw std::runtime_error("TypeError: Cannot print unknown type");
      }
    }
  }
}
