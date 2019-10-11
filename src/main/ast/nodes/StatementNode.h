#pragma once

#include "AstNode.h"

class StatementNode : public AstNode {
  public:
    StatementNode() : AstNode(ast::NodeType::STATEMENT) {}

    void accept(ASTVisitor *v) override;

    bool isReturnStatement();
    void setIsReturnStatement(bool returnStatement);

    AstNode *getChild();
    void setChild(AstNode *child);

  private:
    bool returnStatement = false;
    AstNode *child = nullptr;
};
