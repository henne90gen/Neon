#pragma once

#include <vector>

#include "../../MetaTypes.h"
#include "../AST.h"
#include "../AstNode.h"

class ComplexTypeFinder {
    std::vector<ComplexType> types = {};

  public:
    std::vector<ComplexType> run(AST &tree);

  private:
    void visitNode(AstNode *node);
    void visitSequenceNode(AstNode *node);
    void visitStatementNode(AstNode *node);
    void visitTypeDeclarationNode(AstNode *node);
};
