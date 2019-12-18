#include "TypeResolver.h"

ast::DataType TypeResolver::getTypeOf(AstNode *node) {
    auto itr = typesMap.find(node);
    if (itr == typesMap.end()) {
        return ast::DataType::VOID;
    }
    return itr->second;
}
