#pragma once

#include "Module.h"

#include <llvm/IR/LLVMContext.h>
#include <string>
#include <unordered_map>
#include <vector>

class Program {
  public:
    explicit Program(std::string name, std::vector<std::string> uncompiledModules)
        : name(std::move(name)), uncompiledModules(std::move(uncompiledModules)), objectFileName(this->name + ".o") {}

    std::string name;
    std::string objectFileName;

    std::vector<std::string> uncompiledModules = {};
    std::unordered_map<std::string, Module *> modules = {};
    llvm::LLVMContext llvmContext = {};
};
