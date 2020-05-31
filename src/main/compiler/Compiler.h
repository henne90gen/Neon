#pragma once

#include "../Program.h"
#include "MetaTypes.h"
#include "ast/nodes/FunctionNode.h"

class Compiler {
  public:
    Compiler(Program *program, bool verbose) : program(program), verbose(verbose) {}
    void run();

  private:
    Program *program;
    bool verbose;
    std::unordered_map<Module *, std::vector<std::string>> moduleImportsMap = {};
    std::unordered_map<Module *, std::vector<FunctionSignature>> moduleFunctionsMap = {};
    std::unordered_map<Module *, std::unordered_map<AstNode *, ast::DataType>> moduleNodeToTypeMap;
    std::unordered_map<Module *, std::unordered_map<std::string, ast::DataType>> moduleNameToTypeMap;
    std::unordered_map<Module *, std::vector<ComplexType>> moduleComplexTypesMap;

    Module *loadModule(const std::string &moduleFileName);
    void writeModuleToObjectFile();
    void mergeModules(llvm::Module &destinationModule, const llvm::DataLayout &dataLayout,
                      const std::string &targetTriple);
    void generateIR();
    void analyseTypes();
};
