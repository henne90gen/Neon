#pragma once

#include <utility>

#include "AstNode.h"

class LiteralNode : public AstNode {
  public:
    enum LiteralType { BOOL, INTEGER, FLOAT, STRING };

    explicit LiteralNode(LiteralType type) : AstNode(ast::NodeType::LITERAL), type(type) {}

    LiteralType getLiteralType() { return type; }

  private:
    LiteralType type;
};

class BoolNode : public LiteralNode {
  public:
    explicit BoolNode(bool value) : LiteralNode(LiteralNode::BOOL), value(value) {}

    void accept(AstVisitor *v) override;
    std::string toString() override;

    [[nodiscard]] bool getValue() const { return value; }

  private:
    bool value;
};

class IntegerNode : public LiteralNode {
  public:
    explicit IntegerNode(int value) : LiteralNode(LiteralNode::INTEGER), value(value) {}

    void accept(AstVisitor *v) override;
    std::string toString() override;

    [[nodiscard]] int getValue() const { return value; }

  private:
    int value;
};

class FloatNode : public LiteralNode {
  public:
    explicit FloatNode(float value) : LiteralNode(LiteralNode::FLOAT), value(value) {}

    void accept(AstVisitor *v) override;
    std::string toString() override;

    [[nodiscard]] double getValue() const { return value; }

  private:
    float value;
};

class StringNode : public LiteralNode {
  public:
    explicit StringNode(std::string value) : LiteralNode(LiteralNode::STRING), value(std::move(value)) {}

    void accept(AstVisitor *v) override;
    std::string toString() override;

    [[nodiscard]] std::string getValue() const { return value; }

  private:
    std::string value;
};