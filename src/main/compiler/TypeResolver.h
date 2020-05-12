#pragma once

#include "../Program.h"
#include "MetaTypes.h"
#include "ast/Types.h"
#include "ast/nodes/AstNode.h"

#include <unordered_map>

struct TypeResolveResult {
    bool typeExists = false;
    ComplexType complexType = {.type = ast::DataType()};
    // TODO do we really need this?
    Module *module = nullptr;
};

class TypeResolver {
  public:
    explicit TypeResolver(Program *program, const std::unordered_map<AstNode *, ast::DataType> &typesMap,
                          std::unordered_map<Module *, std::vector<std::string>> &moduleImportsMap,
                          std::unordered_map<Module *, std::vector<ComplexType>> &moduleComplexTypesMap)
        : program(program), nodeToTypeMap(typesMap), moduleImportsMap(moduleImportsMap),
          moduleComplexTypesMap(moduleComplexTypesMap) {}

    ast::DataType getTypeOf(AstNode *node);

    TypeResolveResult resolveType(Module *module, const ast::DataType &type) const;

  private:
    Program *program;

    const std::unordered_map<AstNode *, ast::DataType> &nodeToTypeMap;

    std::unordered_map<Module *, std::vector<std::string>> &moduleImportsMap;
    std::unordered_map<Module *, std::vector<ComplexType>> &moduleComplexTypesMap;
};
