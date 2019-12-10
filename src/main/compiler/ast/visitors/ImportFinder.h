#pragma once

#include "../../../Module.h"
#include "../AstVisitor.h"

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

class ImportFinder : public AstVisitor {
  public:
    explicit ImportFinder(std::filesystem::path directoryPath) : directoryPath(std::move(directoryPath)) {}

    void visitImportNode(ImportNode *node) override;
    void visitSequenceNode(SequenceNode *node) override;
    void visitStatementNode(StatementNode *node) override;

    std::vector<std::string> run(AstNode *rootNode);

    std::filesystem::path directoryPath;
    std::vector<std::string> importedModules = {};
};
