#pragma once

#include "compiler/Token.h"
#include "compiler/ast/nodes/AstNode.h"
#include "compiler/ast/nodes/FunctionNode.h"
#include "compiler/ast/nodes/ImportNode.h"

#include <algorithm>
#include <filesystem>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class Module {
  public:
    explicit Module(std::filesystem::path _filePath, llvm::LLVMContext &context)
        : filePath(std::move(_filePath)), llvmModule(_filePath.string(), context) {}

    [[nodiscard]] std::string toString() const;
    [[nodiscard]] std::string toEscapedString() const;
    [[nodiscard]] std::string toArrayString() const;

    std::string getDirectoryPath() const { return filePath.parent_path(); }
    std::filesystem::path getFilePath() const { return filePath; }

  public:
    AstNode *root = nullptr;
    std::vector<Token> tokens = {};

    llvm::Module llvmModule;

  private:
    std::filesystem::path filePath;
};
