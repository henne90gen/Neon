#pragma once

#include "../Program.h"

#include <string>
#include <unordered_map>

struct FunctionResolveResult {
    bool functionExists;
    ast::DataType dataType;
    std::string module;
};

class FunctionResolver {
  public:
    FunctionResolver(
          Program *program, std::unordered_map<Module *, std::vector<std::string>> &moduleImportsMap,
          std::unordered_map<Module *, std::vector<std::pair<std::string, ast::DataType>>> &moduleFunctionsMap)
        : program(program), moduleImportsMap(moduleImportsMap), moduleFunctionsMap(moduleFunctionsMap) {}

    FunctionResolveResult resolveFunction(Module *module, const std::string &functionName);

    Program *program;
    std::unordered_map<Module *, std::vector<std::string>> &moduleImportsMap;
    std::unordered_map<Module *, std::vector<std::pair<std::string, ast::DataType>>> &moduleFunctionsMap;
};
