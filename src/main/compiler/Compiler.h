#pragma once

#include "../Program.h"
#include "ast/nodes/FunctionNode.h"

class Compiler {
  public:
    Compiler(Program *program, bool verbose) : program(program), verbose(verbose) {}
    void run();

  private:
    Program *program;
    bool verbose;
    std::unordered_map<Module*, std::vector<std::string>> moduleImportsMap;
    std::unordered_map<Module*, std::vector<std::pair<std::string, ast::DataType>>> moduleFunctionsMap;

    Module *loadModule(const std::string &moduleFileName);
    void writeModuleToObjectFile();
    void mergeModules(llvm::Module &module, const llvm::DataLayout &dataLayout, const std::string &targetTriple);
    void generateIR();
};
