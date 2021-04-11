#pragma once

#include "../../../Module.h"
#include "../../MetaTypes.h"
#include "../AST.h"
#include "../AstNode.h"

#include <string>
#include <vector>

class FunctionFinder {
    std::vector<FunctionSignature> functions = {};

  public:
    std::vector<FunctionSignature> run(AST &tree);

  private:
    void visitNode(AstNode *node);
    void visitFunctionNode(FunctionNode *node);
    void visitSequenceNode(SequenceNode *node);
    void visitStatementNode(StatementNode *node);
    void visitTypeDeclarationNode(TypeDeclarationNode *node);
};
