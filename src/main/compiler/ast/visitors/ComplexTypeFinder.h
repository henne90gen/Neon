#pragma once

#include <vector>

#include "../../MetaTypes.h"
#include "../AstVisitor.h"
#include "../nodes/AllNodes.h"

class ComplexTypeFinder : public AstVisitor {
  public:
    void visitSequenceNode(SequenceNode *node) override;
    void visitStatementNode(StatementNode *node) override;
    void visitTypeDeclarationNode(TypeDeclarationNode *node) override;

    std::vector<CompositeType> run(AstNode *rootNode);

  private:
    std::vector<CompositeType> types = {};
};
