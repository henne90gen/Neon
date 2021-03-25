#pragma once

#include "AstNode.h"

class UnaryOperationNode : public AstNode {
  public:

    explicit UnaryOperationNode(ast::UnaryOperationType type) : AstNode(ast::NodeType::UNARY_OPERATION), type(type) {}

    void accept(AstVisitor *v) override;

    AstNode *getChild();

    void setChild(AstNode *child);

    ast::UnaryOperationType getType();

  private:
    ast::UnaryOperationType type;
    AstNode *child = nullptr;
};
