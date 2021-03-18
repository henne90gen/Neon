#pragma once

#include "../Program.h"
#include "MetaTypes.h"
#include "ModuleCompileState.h"

#include <string>
#include <unordered_map>

struct FunctionResolveResult {
    bool functionExists = false;
    FunctionSignature signature = {
          .returnType = ast::DataType(),
    };
    // TODO do we really need this?
    Module *module = nullptr;
};

class FunctionResolver {
  public:
    FunctionResolver(Program *program, std::unordered_map<Module *, ModuleCompileState> &moduleCompileState)
        : program(program), moduleCompileState(moduleCompileState) {}

    FunctionResolveResult resolveFunction(Module *module, const std::string &functionName) const;

    Program *program;
    std::unordered_map<Module *, ModuleCompileState> &moduleCompileState;
};
