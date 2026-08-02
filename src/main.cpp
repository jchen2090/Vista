#include "ast/ast.h"
#include "codegen/codegen.h"
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "token/token.h"
#include "type/typeChecker.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

std::string readFile(std::ifstream &file) {
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

// DEBUGGING FUNCTION
void vectorToString(const std::vector<Token> &vec) {
  std::cout << "[";
  for (size_t i = 0; i < vec.size(); i++) {
    Token token = vec[i];
    token.print();
    if (i < vec.size() - 1) {
      std::cout << ", ";
    }
  }
  std::cout << "]" << std::endl;
}

int main(int argc, char *argv[]) {
  std::string currLine;

  if (argc != 2) {
    throw std::invalid_argument(
        "There must only be the file you want to compile as the argument");
    return -1;
  }
  std::filesystem::path filePath = argv[1];

  if (!(filePath.extension() == ".vista")) {
    throw std::invalid_argument("File must end in .vista");
    return -1;
  }
  std::ifstream fp(filePath);

  if (!fp) {
    throw std::invalid_argument("File does not exist");
    return -1;
  }

  std::string sourceCode = readFile(fp);
  Lexer lexer = Lexer(sourceCode);

  std::vector<Token> tokens = lexer.tokenize();
  vectorToString(tokens);

  Parser parser = Parser(tokens);
  std::unique_ptr<RootNode> ast = parser.parse();
  ast->debugPrint(0);

  TypeChecker tc;
  tc.validate(*ast);

  LLVMGenerator codegen("Vista");
  codegen.generate(*ast);

  std::cout << "\n=== Generated LLVM IR ===\n";
  codegen.dumpIR();

  fp.close();

  return 0;
}
