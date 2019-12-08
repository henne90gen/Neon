#pragma once

#include "compiler/Token.h"
#include "compiler/ast/nodes/AstNode.h"
#include "compiler/ast/nodes/ImportNode.h"
#include "compiler/ast/nodes/FunctionNode.h"

#include <algorithm>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Module {
  public:
    explicit Module(const std::string &fileName, llvm::LLVMContext &context)
        : fileName(fileName), llvmModule(fileName, context) {}

    std::string fileName;
    AstNode *root = nullptr;
    std::vector<Token> tokens = {};

    llvm::Module llvmModule;

    [[nodiscard]] std::string toString() const;
    [[nodiscard]] std::string toEscapedString() const;
    [[nodiscard]] std::string toArrayString() const;
};
