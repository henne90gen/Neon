#pragma once

#include "../../../Module.h"
#include "../AstVisitor.h"

#include <string>
#include <vector>

class FunctionFinder : public AstVisitor {
  public:
    void visitFunctionNode(FunctionNode *node) override;
    void visitSequenceNode(SequenceNode *node) override;
    void visitStatementNode(StatementNode *node) override;

    std::vector<std::pair<std::string, ast::DataType>> run(Module *module);

    std::vector<std::pair<std::string, ast::DataType>> functions = {};
};
