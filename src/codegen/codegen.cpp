#include "codegen.h"
#include "ast/ast.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/Triple.h>
#include <stdexcept>
#include <system_error>

void LLVMGenerator::emitFile(std::string &file) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();

  auto tripleStr = llvm::sys::getDefaultTargetTriple();

  llvm::Triple targetTriple(tripleStr);

  module.setTargetTriple(targetTriple);

  std::string error;
  auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
  if (!target) {
    llvm::errs() << "Error locating target: " << error << "\n";
    return;
  }

  auto cpu = "generic";
  auto features = "";
  llvm::TargetOptions opt;
  auto rm = std::optional<llvm::Reloc::Model>();
  auto targetMachine =
      target->createTargetMachine(targetTriple, cpu, features, opt, rm);

  module.setDataLayout(targetMachine->createDataLayout());

  std::error_code ec;
  llvm::raw_fd_ostream dest(file, ec, llvm::sys::fs::OF_None);
  if (ec) {
    llvm::errs() << "Could not open file: " << ec.message() << "\n";
    return;
  }

  llvm::legacy::PassManager pass;
  auto fileType = llvm::CodeGenFileType::ObjectFile;

  if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
    llvm::errs() << "TargetMachine cannot emit a file of this type\n";
    return;
  }

  pass.run(module);
  dest.flush();
};

llvm::Value *LLVMGenerator::genereateExpressionNodeIR(ExprNode *exprNode) {

  if (auto *intNode = dynamic_cast<IntNode *>(exprNode)) {
    return builder.getInt32(intNode->value);
  } else if (auto *floatNode = dynamic_cast<FloatNode *>(exprNode)) {
    return llvm::ConstantFP::get(builder.getFloatTy(), floatNode->value);
  } else if (auto *strNode = dynamic_cast<StrNode *>(exprNode)) {
    return builder.CreateGlobalString(strNode->value);
  } else if (auto *variableNode = dynamic_cast<IdentifierNode *>(exprNode)) {

    if (symbolTable.find(variableNode->identifier) == symbolTable.end()) {
      throw std::runtime_error("Undeclared variable '" +
                               variableNode->identifier + "'");
    }
    llvm::AllocaInst *alloca = symbolTable[variableNode->identifier];
    return builder.CreateLoad(alloca->getAllocatedType(), alloca,
                              variableNode->identifier);
  }
  throw std::runtime_error("Unsupported AST node for LLVM IR generation");
}

void LLVMGenerator::createMainFn() {
  llvm::FunctionType *funcType =
      llvm::FunctionType::get(builder.getInt32Ty(), false);
  llvm::Function *mainFunc = llvm::Function::Create(
      funcType, llvm::Function::ExternalLinkage, "main", module);

  llvm::BasicBlock *entryBlock =
      llvm::BasicBlock::Create(context, "entry", mainFunc);

  builder.SetInsertPoint(entryBlock);
}

void LLVMGenerator::generate(RootNode &root) {
  createMainFn();

  for (std::unique_ptr<ASTNode> &node : root.statements) {
    if (auto *assignmentNode =
            dynamic_cast<AssignmentStatement *>(node.get())) {

      llvm::Value *val = genereateExpressionNodeIR(assignmentNode->value.get());

      // If variable doesn't exist, allocate stack memory
      if (symbolTable.find(assignmentNode->identifier) == symbolTable.end()) {
        llvm::AllocaInst *alloca = builder.CreateAlloca(
            val->getType(), nullptr, assignmentNode->identifier);
        symbolTable[assignmentNode->identifier] = alloca;
      }

      builder.CreateStore(val, symbolTable[assignmentNode->identifier]);
    }

    else if (auto *printNode = dynamic_cast<PrintStatement *>(node.get())) {
      llvm::Value *val = genereateExpressionNodeIR(printNode->value.get());

      llvm::FunctionType *printfType = llvm::FunctionType::get(
          builder.getInt32Ty(), {builder.getPtrTy()}, true);
      llvm::FunctionCallee printfFunc =
          module.getOrInsertFunction("printf", printfType);

      llvm::Value *formatStrPtr = nullptr;

      if (val->getType()->isIntegerTy(32)) {
        formatStrPtr = builder.CreateGlobalString("%d\n");
      } else if (val->getType()->isFloatTy()) {
        val = builder.CreateFPExt(val, builder.getDoubleTy());
        formatStrPtr = builder.CreateGlobalString("%f\n");
      } else if (val->getType()->isPointerTy()) {
        formatStrPtr = builder.CreateGlobalString("%s\n");
      } else {
        throw std::runtime_error("Unsupported type for print statement");
      }

      builder.CreateCall(printfFunc, {formatStrPtr, val});
    }
  }
  builder.CreateRet(builder.getInt32(0));
}
