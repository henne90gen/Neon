#include "FunctionResolver.h"

FunctionResolveResult FunctionResolver::resolveFunction(Module *module, const std::string &functionName) {
    FunctionResolveResult result = {false};

    const auto &moduleId = moduleImportsMap[module];
    for (const auto &importedModuleId : moduleId) {
        auto importedModule = program->modules[importedModuleId];
        for (const auto &function : moduleFunctionsMap[importedModule]) {
            if (function.first == functionName) {
                result.functionExists = true;
                result.dataType = function.second;
                result.module = module->fileName;
                return result;
            }
        }
    }

    return result;
}
