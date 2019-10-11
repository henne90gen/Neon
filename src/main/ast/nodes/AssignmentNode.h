#pragma once

#include "AstNode.h"

class AssignmentNode : public AstNode {
  public:
    explicit AssignmentNode() : AstNode(ast::NodeType::ASSIGNMENT) {}

    void accept(ASTVisitor *v) override;

    AstNode *getLeft();
    void setLeft(AstNode *left);

    AstNode *getRight();
    void setRight(AstNode *right);

  private:
    AstNode *left = nullptr;
    AstNode *right = nullptr;
};
