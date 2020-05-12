#include "TypeResolver.h"

ast::DataType TypeResolver::getTypeOf(AstNode *node) {
    auto itr = nodeToTypeMap.find(node);
    if (itr == nodeToTypeMap.end()) {
        return ast::DataType(ast::SimpleDataType::VOID);
    }
    return itr->second;
}

TypeResolveResult TypeResolver::resolveType(Module *module, const ast::DataType &type) const {
    TypeResolveResult result = {false};

    for (const auto &complexType : moduleComplexTypesMap[module]) {
        if (complexType.type == type) {
            result.typeExists = true;
            result.module = module;
            result.complexType = complexType;
            return result;
        }
    }

    const auto &moduleIds = moduleImportsMap[module];
    for (const auto &importedModuleId : moduleIds) {
        auto importedModule = program->modules[importedModuleId];
        for (const auto &complexType : moduleComplexTypesMap[importedModule]) {
            if (complexType.type == type) {
                result.typeExists = true;
                result.module = importedModule;
                result.complexType = complexType;
                return result;
            }
        }
    }

    return result;
}
