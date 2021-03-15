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
    /**
     * @return symbol with a single whitespace on either side that represents this binary operation, e.g. " == ".
     */
    std::string operationToString();

  private:
    ast::BinaryOperationType type;
    AstNode *left = nullptr;
    AstNode *right = nullptr;
};
