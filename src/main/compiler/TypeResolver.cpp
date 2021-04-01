#include "TypeResolver.h"

ast::DataType TypeResolver::getTypeOf(Module *module, AstNode *node) {
    std::unordered_map<AstNode *, ast::DataType> &nodeToTypeMap = moduleCompileState[module].nodeToTypeMap;
    auto itr = nodeToTypeMap.find(node);
    if (itr == nodeToTypeMap.end()) {
        return ast::DataType(ast::SimpleDataType::VOID);
    }
    return itr->second;
}

ast::DataType TypeResolver::getTypeOf(Module *module, const std::string &variableName) {
    std::unordered_map<std::string, ast::DataType> &nameToTypeMap = moduleCompileState[module].nameToTypeMap;
    auto itr = nameToTypeMap.find(variableName);
    if (itr == nameToTypeMap.end()) {
        return ast::DataType(ast::SimpleDataType::VOID);
    }
    return itr->second;
}

TypeResolveResult TypeResolver::resolveType(Module *module, const ast::DataType &type) const {
    TypeResolveResult result = {false};

    for (const auto &complexType : moduleCompileState[module].complexTypes) {
        if (complexType.type == type) {
            result.typeExists = true;
            result.module = module;
            result.complexType = complexType;
            return result;
        }
    }

    const auto &moduleIds = moduleCompileState[module].imports;
    for (const auto &importedModuleId : moduleIds) {
        auto *importedModule = program->modules[importedModuleId];
        for (const auto &complexType : moduleCompileState[importedModule].complexTypes) {
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
