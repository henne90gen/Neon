#pragma once

#include "ast/Types.h"
#include "ast/nodes/AstNode.h"

#include <unordered_map>

class TypeResolver {
  public:
    explicit TypeResolver(const std::unordered_map<AstNode *, ast::DataType> &typesMap) : typesMap(typesMap) {}
    ast::DataType getTypeOf(AstNode *node);

  private:
    const std::unordered_map<AstNode *, ast::DataType> &typesMap;
};
