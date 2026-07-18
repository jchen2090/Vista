#ifndef AST_H
#define AST_H

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

class ASTNode {
public:
  virtual ~ASTNode() = default;
  virtual void debugPrint(int indent = 0) const = 0;
};

// Represents any piece of code that evaluates to a literal value (e.g string,
// double, int)
class ExprNode : public ASTNode {
public:
  virtual ~ExprNode() = default;
};

class AssignmentStatement : public ASTNode {
public:
  std::string typeAnnotaiton;
  std::string identifier;
  std::unique_ptr<ExprNode> value;

  explicit AssignmentStatement(std::string type, std::string id,
                               std::unique_ptr<ExprNode> val)
      : typeAnnotaiton(std::move(type)), identifier(std::move(id)),
        value(std::move(val)) {};

  void debugPrint(int indent) const override;
};

class PrintStatement : public ASTNode {
public:
  std::unique_ptr<ExprNode> value;

  explicit PrintStatement(std::unique_ptr<ExprNode> val)
      : value(std::move(val)) {};

  void debugPrint(int indent) const override;
};

class IdentifierNode : public ExprNode {
public:
  std::string identifier;

  explicit IdentifierNode(std::string id) : identifier(std::move(id)) {};

  void debugPrint(int indent) const override;
};

class IntNode : public ExprNode {
public:
  int value;
  explicit IntNode(int val) : value(val) {};

  void debugPrint(int indent) const override;
};

class FloatNode : public ExprNode {
public:
  double value;
  explicit FloatNode(double val) : value(val) {};

  void debugPrint(int indent) const override;
};

class StrNode : public ExprNode {
public:
  std::string value;
  explicit StrNode(std::string val) : value(std::move(val)) {};

  void debugPrint(int indent) const override;
};

class RootNode : public ASTNode {
public:
  std::vector<std::unique_ptr<ASTNode>> statements;

  void debugPrint(int indent) const override;
};

#endif
