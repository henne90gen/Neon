#pragma once

#include "AstNode.h"
#include "../Types.h"

class BinaryOperationNode : public AstNode {
  public:

    explicit BinaryOperationNode(ast::BinaryOperationType type) : AstNode(ast::NodeType::BINARY_OPERATION), type(type) {}

    void accept(AstVisitor *v) override;
    std::string toString() override;

    AstNode *getLeft();
    void setLeft(AstNode *left);

    void setRight(AstNode *right);
    AstNode *getRight();

    ast::BinaryOperationType getType();
    std::string operationToString();

  private:
    ast::BinaryOperationType type;
    AstNode *left = nullptr;
    AstNode *right = nullptr;
};
