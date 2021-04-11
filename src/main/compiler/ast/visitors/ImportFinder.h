#pragma once

#include "../../../Module.h"
#include "../AST.h"
#include "../AstNode.h"

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

class ImportFinder {
    std::filesystem::path directoryPath;
    std::vector<std::string> importedModules = {};

  public:
    explicit ImportFinder(std::filesystem::path directoryPath) : directoryPath(std::move(directoryPath)) {}

    std::vector<std::string> run(AST &tree);

  private:
    void visitNode(AstNode *node);
    void visitImportNode(ImportNode *node);
    void visitSequenceNode(SequenceNode *node);
    void visitStatementNode(StatementNode *node);
};
