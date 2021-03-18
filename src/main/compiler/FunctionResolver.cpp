#include "FunctionResolver.h"

FunctionResolveResult FunctionResolver::resolveFunction(Module *module, const std::string &functionName) const {
    FunctionResolveResult result = {.functionExists = false};

    // look inside the current module first
    for (const auto &function : moduleCompileState[module].functions) {
        if (function.name == functionName) {
            result.functionExists = true;
            result.module = module;
            result.signature = function;
            return result;
        }
    }

    // then check all the imported modules
    const auto &moduleIds = moduleCompileState[module].imports;
    for (const auto &importedModuleId : moduleIds) {
        auto importedModule = program->modules[importedModuleId];
        for (const auto &function : moduleCompileState[importedModule].functions) {
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
