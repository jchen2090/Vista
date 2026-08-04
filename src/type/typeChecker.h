#ifndef TYPE_CHECKER_H
#define TYPE_CHECKER_H

#include <map>
#include <string>

#include "../ast/ast.h"
#include "types.h"

class TypeChecker {
private:
  std::map<std::string, Type> symbolTable = {};

  std::string typeToString(Type);

public:
  void validate(RootNode &root);
};

#endif
