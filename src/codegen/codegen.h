#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <unordered_map>

#include "../ast/ast.h"

class LLVMGenerator {
private:
  llvm::LLVMContext context;
  llvm::Module module;
  llvm::IRBuilder<> builder;

  std::unordered_map<std::string, llvm::AllocaInst *> symbolTable;

  llvm::Value *genereateExpressionNodeIR(ExprNode *);
  void createMainFn();

public:
  LLVMGenerator(const std::string &moduleName)
      : module(moduleName, context), builder(context) {}

  void generate(RootNode &root);
  void dumpIR() const { module.print(llvm::errs(), nullptr); }
};
