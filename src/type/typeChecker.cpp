#include "typeChecker.h"
#include "types.h"
#include <memory>
#include <stdexcept>

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

void TypeChecker::validate(RootNode &root) {
  for (std::unique_ptr<ASTNode> &node : root.statements) {

    // Checks typing for variable declaration and initialization
    if (AssignmentStatement *assignmentNode =
            dynamic_cast<AssignmentStatement *>(node.get())) {

      Type lhs = assignmentNode->typeAnnotation;
      Type rhs = assignmentNode->value->getExpression();

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

      Type val = printNode->value->getExpression();

      if (val == Type::UNKNOWN) {
        throw std::runtime_error("TypeError: Cannot print unknown type");
      }
    }
  }
}
