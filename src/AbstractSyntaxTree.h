#pragma once

#include <utility>

#include "Parser.h"

class AstNode {
  public:
    enum AstNodeType { SEQUENCE, STATEMENT, LITERAL, UNARY_OPERATION, BINARY_OPERATION };
    explicit AstNode(AstNodeType type) : type(type) {}

    virtual void print(int indentation = 0) = 0;

    virtual void generateIR() = 0;

    AstNodeType getAstNodeType() { return type; }

  private:
    AstNodeType type;
};

class SequenceNode : public AstNode {
  public:
    SequenceNode() : AstNode(AstNode::SEQUENCE) {}

    void print(int indentation) override;

    void generateIR() override {}

    std::vector<AstNode *> &getChildren() { return children; }

  private:
    std::vector<AstNode *> children = {};
};

class StatementNode : public AstNode {
  public:
    StatementNode() : AstNode(AstNode::STATEMENT) {}

    void print(int indentation) override;

    void generateIR() override {}

    void setChild(AstNode *child) { this->child = child; }

    AstNode *getChild() { return child; }

  private:
    AstNode *child = nullptr;
};

class LiteralNode : public AstNode {
  public:
    enum LiteralType { INTEGER, FLOAT, BOOL };

    explicit LiteralNode(LiteralType type) : AstNode(AstNode::LITERAL), type(type) {}

    LiteralType getLiteralType() { return type; }

  private:
    LiteralType type;
};

class IntegerNode : public LiteralNode {
  public:
    explicit IntegerNode(int value) : LiteralNode(LiteralNode::INTEGER), value(value) {}

    int getValue() { return value; }

    void print(int indentation) override;

    void generateIR() override {}

  private:
    int value;
};

class FloatNode : public LiteralNode {
  public:
    explicit FloatNode(float value) : LiteralNode(LiteralNode::FLOAT), value(value) {}

    float getValue() { return value; }

    void print(int indentation) override;

    void generateIR() override {}

  private:
    float value;
};

class BoolNode : public LiteralNode {
  public:
    explicit BoolNode(bool value) : LiteralNode(LiteralNode::BOOL), value(value) {}

    bool getValue() { return value; }

    void print(int indentation) override;

    void generateIR() override {}

  private:
    bool value;
};

class UnaryOperationNode : public AstNode {
  public:
    enum UnaryOperationType {
        NOT,
    };

    explicit UnaryOperationNode(UnaryOperationType type) : AstNode(AstNode::UNARY_OPERATION), type(type) {}

    void setChild(AstNode *child) { this->child = child; }

    AstNode *getChild() { return child; }

    UnaryOperationType getType() { return type; }

    void print(int indentation) override;

    void generateIR() override {}

  private:
    UnaryOperationType type;
    AstNode *child = nullptr;
};

class BinaryOperationNode : public AstNode {
  public:
    enum BinaryOperationType {
        ADDITION,
        MULTIPLICATION,
        SUBTRACTION,
        DIVISION,
    };

    explicit BinaryOperationNode(BinaryOperationType type) : AstNode(AstNode::BINARY_OPERATION), type(type) {}

    void setLeft(AstNode *left) { this->left = left; }
    AstNode *getLeft() { return left; }

    void setRight(AstNode *right) { this->right = right; }
    AstNode *getRight() { return right; }

    BinaryOperationType getType() { return type; }

    void print(int indentation) override;

    void generateIR() override {}

  private:
    BinaryOperationType type;
    AstNode *left = nullptr;
    AstNode *right = nullptr;
};

AstNode *createAstFromParseTree(ParseTreeNode *node);

// void printAst(AstNode *node, int indentation = 0);
