#pragma once

#include "AstNode.h"

class LiteralNode : public AstNode {
  public:
    enum LiteralType { INTEGER, FLOAT, BOOL };

    explicit LiteralNode(LiteralType type) : AstNode(ast::NodeType::LITERAL), type(type) {}

    LiteralType getLiteralType() { return type; }

  private:
    LiteralType type;
};

class IntegerNode : public LiteralNode {
  public:
    explicit IntegerNode(int value) : LiteralNode(LiteralNode::INTEGER), value(value) {}

    void accept(AstVisitor *v) override;

    int getValue() { return value; }

  private:
    int value;
};

class FloatNode : public LiteralNode {
  public:
    explicit FloatNode(float value) : LiteralNode(LiteralNode::FLOAT), value(value) {}

    void accept(AstVisitor *v) override;

    double getValue() { return value; }

  private:
    float value;
};

class BoolNode : public LiteralNode {
  public:
    explicit BoolNode(bool value) : LiteralNode(LiteralNode::BOOL), value(value) {}

    void accept(AstVisitor *v) override;

    bool getValue() { return value; }

  private:
    bool value;
};
