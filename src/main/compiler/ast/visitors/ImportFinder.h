#pragma once

#include "../../../Module.h"
#include "../AstVisitor.h"

#include <string>
#include <vector>

class ImportFinder : public AstVisitor {
  public:
    void visitImportNode(ImportNode *node) override;
    void visitSequenceNode(SequenceNode *node) override;
    void visitStatementNode(StatementNode *node) override;

    std::vector<std::string> run(Module *module);

    std::vector<std::string> importedModules = {};
};
