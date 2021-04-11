#pragma once

#include "compiler/ast/AST.h"
#include "compiler/lexer/Lexer.h"
#include "compiler/lexer/Token.h"

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
        : filePath(std::move(_filePath)), codeProvider(new FileCodeProvider(_filePath)),
          llvmModule(_filePath.string(), context) {}

    [[nodiscard]] std::string toString() const;
    [[nodiscard]] std::string toEscapedString() const;
    [[nodiscard]] std::string toArrayString() const;

    std::string getDirectoryPath() const { return filePath.parent_path().string(); }
    std::filesystem::path getFilePath() const { return filePath; }

    CodeProvider *getCodeProvider() const;

  public:
    CodeProvider *codeProvider;

    AST ast;
    std::vector<Token> tokens = {};

    llvm::Module llvmModule;

  private:
    std::filesystem::path filePath;
};
