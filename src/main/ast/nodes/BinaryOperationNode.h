#pragma once

#include "AstNode.h"

class BinaryOperationNode : public AstNode {
  public:
    enum BinaryOperationType {
        ADDITION,
        MULTIPLICATION,
        SUBTRACTION,
        DIVISION,
    };

    explicit BinaryOperationNode(BinaryOperationType type) : AstNode(ast::NodeType::BINARY_OPERATION), type(type) {}

    void accept(ASTVisitor *v) override;

    AstNode *getLeft();
    void setLeft(AstNode *left);

    void setRight(AstNode *right);
    AstNode *getRight();

    BinaryOperationType getType();

  private:
    BinaryOperationType type;
    AstNode *left = nullptr;
    AstNode *right = nullptr;
};
