#pragma once

struct ModuleCompileState {
    std::vector<std::string> imports = {};
    std::vector<FunctionSignature> functions = {};
    std::unordered_map<AstNode*, ast::DataType> nodeToTypeMap;
    std::unordered_map<std::string, ast::DataType> nameToTypeMap;
    std::vector<ComplexType> complexTypes;
};
