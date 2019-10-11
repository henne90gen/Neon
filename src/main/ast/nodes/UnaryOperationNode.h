#pragma once

#include "AstNode.h"

class UnaryOperationNode : public AstNode {
  public:
    enum UnaryOperationType {
        NOT,
    };

    explicit UnaryOperationNode(UnaryOperationType type) : AstNode(ast::NodeType::UNARY_OPERATION), type(type) {}

    void accept(ASTVisitor *v) override;

    AstNode *getChild();

    void setChild(AstNode *child);

    UnaryOperationType getType();

  private:
    UnaryOperationType type;
    AstNode *child = nullptr;
};
