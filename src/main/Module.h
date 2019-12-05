#pragma once

#include "Token.h"
#include "ast/nodes/AstNode.h"

#include <algorithm>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Module {
  public:
    explicit Module(std::string fileName, llvm::LLVMContext &context)
        : fileName(fileName), llvmModule(fileName, context) {}

    std::string fileName;
    AstNode *root = nullptr;
    std::vector<Token> tokens = {};

    llvm::Module llvmModule;

    [[nodiscard]] std::string toString() const;
    [[nodiscard]] std::string toEscapedString() const;
    [[nodiscard]] std::string toArrayString() const;
};

class Program {
  public:
    explicit Program(std::string name, std::vector<std::string> uncompiledModules)
        : name(std::move(name)), uncompiledModules(std::move(uncompiledModules)), objectFileName(this->name + ".o") {}

    std::string name;
    std::string objectFileName;

    std::vector<std::string> uncompiledModules = {};
    std::unordered_map<std::string, Module*> modules = {};
    llvm::LLVMContext llvmContext = {};
};
