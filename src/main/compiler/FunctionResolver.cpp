#include "FunctionResolver.h"

FunctionResolveResult FunctionResolver::resolveFunction(Module *module, const std::string &functionName) const {
    FunctionResolveResult result = {false};

    for (const auto &function : moduleFunctionsMap[module]) {
        if (function.name == functionName) {
            result.functionExists = true;
            result.module = module;
            result.signature = function;
            return result;
        }
    }

    const auto &moduleIds = moduleImportsMap[module];
    for (const auto &importedModuleId : moduleIds) {
        auto importedModule = program->modules[importedModuleId];
        for (const auto &function : moduleFunctionsMap[importedModule]) {
            if (function.name == functionName) {
                result.functionExists = true;
                result.module = importedModule;
                result.signature = function;
                return result;
            }
        }
    }

    return result;
}
