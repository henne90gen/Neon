#pragma once

#include "../Program.h"
#include "MetaTypes.h"
#include "ModuleCompileState.h"
#include "ast/Types.h"
#include "ast/nodes/AstNode.h"

#include <unordered_map>

struct TypeResolveResult {
    bool typeExists = false;
    CompositeType complexType = {ast::DataType()};
    // TODO do we really need this?
    Module *module = nullptr;
};

class TypeResolver {
  public:
    explicit TypeResolver(Program *program, std::unordered_map<Module *, ModuleCompileState> &moduleCompileState)
        : program(program), moduleCompileState(moduleCompileState) {}

    ast::DataType getTypeOf(Module *module, AstNode *node);
    ast::DataType getTypeOf(Module *module, const std::string &variableName);

    TypeResolveResult resolveType(Module *module, const ast::DataType &type) const;

  private:
    Program *program;

    std::unordered_map<Module *, ModuleCompileState> &moduleCompileState;
};
