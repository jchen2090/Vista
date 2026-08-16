#include "typeChecker.h"
#include "ast/ast.h"
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
  case Type::IDENTIFIER:
    return "identifier";
  case Type::UNKNOWN:
    return "unknown";
  default:
    throw std::runtime_error("Unknown enum type provided");
  }
}

// TODO: This class needs a major refactor...Look into visitor pattern
void TypeChecker::validate(RootNode &root) {
  for (std::unique_ptr<ASTNode> &node : root.statements) {

    // Checks typing for variable declaration and initialization
    if (AssignmentStatement *assignmentNode =
            dynamic_cast<AssignmentStatement *>(node.get())) {

      Type lhs = assignmentNode->typeAnnotation;
      Type rhs = assignmentNode->value->getType();

      std::string identifier = assignmentNode->identifier;

      if (symbolTable.find(identifier) != symbolTable.end()) {
        std::string msg =
            "Error: variable '" + identifier + "' has already been declared";
        throw std::runtime_error(msg);
      }

      if (rhs == Type::IDENTIFIER) {
        auto identifierNode =
            dynamic_cast<IdentifierNode *>(assignmentNode->value.get());

        if (!identifierNode ||
            symbolTable.find(identifierNode->identifier) == symbolTable.end()) {

          std::string msg = "TypeError: Variable '" +
                            identifierNode->identifier + "' not declared";
          throw std::runtime_error(msg);
        }

        rhs = symbolTable[identifierNode->identifier];
      }

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

      Type val = printNode->value->getType();

      if (val == Type::UNKNOWN) {
        throw std::runtime_error("TypeError: Cannot print unknown type");
      }

      if (val == Type::IDENTIFIER) {
        auto identifierNode =
            dynamic_cast<IdentifierNode *>(printNode->value.get());

        if (!identifierNode ||
            symbolTable.find(identifierNode->identifier) == symbolTable.end()) {

          std::string msg = "TypeError: Variable '" +
                            identifierNode->identifier + "' not declared";
          throw std::runtime_error(msg);
        }
      }

      // TODO: PLEASE REFACTOR THIS
      if (val == Type::BIN_OP) {
        auto binaryOp = dynamic_cast<BinaryOpNode *>(printNode->value.get());
        std::string opString;

        if (binaryOp->op == BinaryOp::ADD) {
          opString = "add";
        } else if (binaryOp->op == BinaryOp::SUBTRACT) {
          opString = "subtract";
        } else if (binaryOp->op == BinaryOp::MULTIPLY) {
          opString = "multiply";
        } else if (binaryOp->op == BinaryOp::DIVIDE) {
          opString = "divide";
        }
        Type lhsType = binaryOp->lhs->getType();
        Type rhsType = binaryOp->rhs->getType();

        std::string errMsg = "TypeError: Cannot " + opString + " type " +
                             typeToString(lhsType) + " and type " +
                             typeToString(rhsType);

        if (lhsType == Type::STR || rhsType == Type::STR) {
          throw std::runtime_error(errMsg);
        } else if (lhsType != rhsType) {
          throw std::runtime_error(errMsg);
        } else if (lhsType == Type::IDENTIFIER || rhsType == Type::IDENTIFIER) {
          auto *lhs = dynamic_cast<IdentifierNode *>(binaryOp->lhs.get());
          auto *rhs = dynamic_cast<IdentifierNode *>(binaryOp->rhs.get());

          if (!lhs || !rhs) {
            throw std::runtime_error(
                "TypeError: Cannot cast binOp LHS and RHS to identifierNodes");
          }

          if (symbolTable.find(lhs->identifier) == symbolTable.end()) {
            throw std::runtime_error("TypeError: variable '" + lhs->identifier +
                                     "' is unknown");
          }

          if (symbolTable.find(rhs->identifier) == symbolTable.end()) {
            throw std::runtime_error("TypeError: variable '" + rhs->identifier +
                                     "' is unknown");
          }

          Type lhsVarType = symbolTable[lhs->identifier];
          Type rhsVarType = symbolTable[rhs->identifier];

          errMsg = "TypeError: Cannot " + opString + " type " +
                   typeToString(lhsVarType) + " and type " +
                   typeToString(rhsVarType);

          if (lhsVarType == Type::STR || rhsVarType == Type::STR) {
            throw std::runtime_error(errMsg);
          } else if (lhsVarType != rhsVarType) {
            throw std::runtime_error(errMsg);
          }
        }
      }

    } else if (auto *binaryOP = dynamic_cast<BinaryOpNode *>(node.get())) {
      std::string opString;

      if (binaryOP->op == BinaryOp::ADD) {
        opString = "add";
      } else if (binaryOP->op == BinaryOp::SUBTRACT) {
        opString = "subtract";
      } else if (binaryOP->op == BinaryOp::MULTIPLY) {
        opString = "multiply";
      } else if (binaryOP->op == BinaryOp::DIVIDE) {
        opString = "divide";
      }
      Type lhsType = binaryOP->lhs->getType();
      Type rhsType = binaryOP->rhs->getType();

      std::string errMsg = "TypeError: Cannot " + opString + " type " +
                           typeToString(lhsType) + " and type " +
                           typeToString(rhsType);

      if (lhsType == Type::STR || rhsType == Type::STR) {
        throw std::runtime_error(errMsg);
      } else if (lhsType != rhsType) {
        throw std::runtime_error(errMsg);
      } else if (lhsType == Type::IDENTIFIER || rhsType == Type::IDENTIFIER) {
        auto *lhs = dynamic_cast<IdentifierNode *>(binaryOP->lhs.get());
        auto *rhs = dynamic_cast<IdentifierNode *>(binaryOP->rhs.get());

        if (!lhs || !rhs) {
          throw std::runtime_error(
              "TypeError: Cannot cast binOp LHS and RHS to identifierNodes");
        }

        if (symbolTable.find(lhs->identifier) == symbolTable.end()) {
          throw std::runtime_error("TypeError: variable '" + lhs->identifier +
                                   "' is unknown");
        }

        if (symbolTable.find(rhs->identifier) == symbolTable.end()) {
          throw std::runtime_error("TypeError: variable '" + rhs->identifier +
                                   "' is unknown");
        }

        Type lhsVarType = symbolTable[lhs->identifier];
        Type rhsVarType = symbolTable[rhs->identifier];

        errMsg = "TypeError: Cannot " + opString + " type " +
                 typeToString(lhsVarType) + " and type " +
                 typeToString(rhsVarType);

        if (lhsVarType == Type::STR || rhsVarType == Type::STR) {
          throw std::runtime_error(errMsg);
        } else if (lhsVarType != rhsVarType) {
          throw std::runtime_error(errMsg);
        }
      }
    }
  }
}
