#pragma once

class AssignmentNode : public AstNode {
  public:
    explicit AssignmentNode() : AstNode(AstNode::ASSIGNMENT) {}

    void accept(ASTVisitor *v) override { v->visitAssignmentNode(this); };

    AstNode *getLeft() { return left; }
    void setLeft(AstNode *left) { this->left = left; }

    AstNode *getRight() { return right; }
    void setRight(AstNode *right) { this->right = right; }

  private:
    AstNode *left = nullptr;
    AstNode *right = nullptr;
};
