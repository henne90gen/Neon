#pragma once

class UnaryOperationNode : public AstNode {
  public:
    enum UnaryOperationType {
        NOT,
    };

    explicit UnaryOperationNode(UnaryOperationType type) : AstNode(AstNode::UNARY_OPERATION), type(type) {}

    void accept(ASTVisitor *v) override { v->visitUnaryOperationNode(this); };

    void setChild(AstNode *child) { this->child = child; }

    AstNode *getChild() { return child; }

    UnaryOperationType getType() { return type; }

  private:
    UnaryOperationType type;
    AstNode *child = nullptr;
};
