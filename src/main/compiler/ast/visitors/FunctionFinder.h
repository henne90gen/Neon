#pragma once

#include "../../../Module.h"
#include "../../MetaTypes.h"
#include "../AstVisitor.h"

#include <string>
#include <vector>

class FunctionFinder : public AstVisitor {
  public:
    void visitFunctionNode(FunctionNode *node) override;
    void visitSequenceNode(SequenceNode *node) override;
    void visitStatementNode(StatementNode *node) override;
    void visitTypeDeclarationNode(TypeDeclarationNode *node) override;

    std::vector<FunctionSignature> run(AstNode *rootNode);

    std::vector<FunctionSignature> functions = {};
};
