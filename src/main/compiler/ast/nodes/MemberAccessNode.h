#pragma once

#include <string>
#include <utility>
#include <vector>

#include "AstNode.h"

class VariableNode;

class MemberAccessNode : public AstNode {
  public:
    explicit MemberAccessNode() : AstNode(ast::NodeType::MEMBER_ACCESS) {}

    void accept(AstVisitor *visitor) override;

    AstNode *getLeft() { return left; }
    void setLeft(AstNode *l) { this->left = l; }

    AstNode *getRight() { return right; }
    void setRight(AstNode *r) { this->right = r; }

    std::vector<VariableNode*> linearizeAccessTree();

  private:
    AstNode *left = nullptr;
    AstNode *right = nullptr;
};
