#pragma once

class BinaryOperationNode : public AstNode {
  public:
    enum BinaryOperationType {
        ADDITION,
        MULTIPLICATION,
        SUBTRACTION,
        DIVISION,
    };

    explicit BinaryOperationNode(BinaryOperationType type) : AstNode(AstNode::BINARY_OPERATION), type(type) {}

    void accept(ASTVisitor *v) override { v->visitBinaryOperationNode(this); };

    void setLeft(AstNode *left) { this->left = left; }
    AstNode *getLeft() { return left; }

    void setRight(AstNode *right) { this->right = right; }
    AstNode *getRight() { return right; }

    BinaryOperationType getType() { return type; }

  private:
    BinaryOperationType type;
    AstNode *left = nullptr;
    AstNode *right = nullptr;
};
