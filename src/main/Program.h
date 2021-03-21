#pragma once

#include "Module.h"

#include <llvm/IR/LLVMContext.h>
#include <string>
#include <unordered_map>
#include <vector>

class Program {
  public:
    explicit Program(std::string entryPoint);
    explicit Program() = default;

    std::string entryPoint;
    std::string name;

    std::unordered_map<std::string, Module *> modules = {};
    llvm::LLVMContext llvmContext = {};

    [[nodiscard]] std::string objectFileName() const;
    [[nodiscard]] std::string executableFileName() const;
};
