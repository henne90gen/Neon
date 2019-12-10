#include "FunctionResolver.h"

FunctionResolveResult FunctionResolver::resolveFunction(Module *module, const std::string &functionName) {
    FunctionResolveResult result = {false};

    for (const auto &function : moduleFunctionsMap[module]) {
        if (function.first == functionName) {
            result.functionExists = true;
            result.dataType = function.second;
            result.module = module;
            return result;
        }
    }

    const auto &moduleIds = moduleImportsMap[module];
    for (const auto &importedModuleId : moduleIds) {
        auto importedModule = program->modules[importedModuleId];
        for (const auto &function : moduleFunctionsMap[importedModule]) {
            if (function.first == functionName) {
                result.functionExists = true;
                result.dataType = function.second;
                result.module = importedModule;
                return result;
            }
        }
    }

    return result;
}
