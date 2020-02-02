#pragma once

#include <functional>
#include <llvm/IR/Value.h>
#include <unordered_map>
#include <vector>

class Scope {
  public:
    Scope() = default;

    std::unordered_map<std::string, llvm::Value *> definedVariables = {};

    // TODO find a better name
    std::vector<std::function<void(void)>> cleanUpFunctions = {};
};
