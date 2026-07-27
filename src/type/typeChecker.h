#ifndef TYPE_H
#define TYPE_H

#include <map>
#include <memory>
#include <string>

#include "../ast/ast.h"

enum class Type { INT, STR, FLOAT, UNKNOWN };

class TypeChecker {
private:
  std::map<std::string, Type> symbolTable = {};
  std::unique_ptr<RootNode> root;

  // TODO: Potentially change this to an enum?
  Type parseExpression(std::unique_ptr<ExprNode>);
  Type stringToType(std::string);
  std::string typeToString(Type);

public:
  TypeChecker(std::unique_ptr<RootNode>);
  void validate();
};

#endif
