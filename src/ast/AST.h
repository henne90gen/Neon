#pragma once

#include <string>
#include <utility>
#include <vector>

class FunctionNode;
class VariableNode;
class VariableDefinitionNode;
class BinaryOperationNode;
class UnaryOperationNode;
class AssignmentNode;
class SequenceNode;
class StatementNode;
class FloatNode;
class IntegerNode;
class BoolNode;
class CallNode;

class ASTVisitor {
  public:
    virtual void visitFunctionNode(FunctionNode *node) = 0;
    virtual void visitCallNode(CallNode *node) = 0;
    virtual void visitVariableNode(VariableNode *node) = 0;
    virtual void visitVariableDefinitionNode(VariableDefinitionNode *node) = 0;
    virtual void visitUnaryOperationNode(UnaryOperationNode *node) = 0;
    virtual void visitBinaryOperationNode(BinaryOperationNode *node) = 0;
    virtual void visitAssignmentNode(AssignmentNode *node) = 0;
    virtual void visitSequenceNode(SequenceNode *node) = 0;
    virtual void visitStatementNode(StatementNode *node) = 0;
    virtual void visitFloatNode(FloatNode *node) = 0;
    virtual void visitIntegerNode(IntegerNode *node) = 0;
    virtual void visitBoolNode(BoolNode *node) = 0;
};

class AstNode {
  public:
    enum AstNodeType {
        SEQUENCE,
        STATEMENT,
        LITERAL,
        UNARY_OPERATION,
        BINARY_OPERATION,
        FUNCTION,
        CALL,
        VARIABLE_DEFINITION,
        VARIABLE,
        ASSIGNMENT,
    };
    enum DataType { VOID, INT, FLOAT, BOOL };

    explicit AstNode(AstNodeType type) : type(type) {}

    virtual void accept(ASTVisitor *v) = 0;

    AstNodeType getAstNodeType() { return type; }

  private:
    AstNodeType type;
};

class SequenceNode : public AstNode {
  public:
    SequenceNode() : AstNode(AstNode::SEQUENCE) {}

    void accept(ASTVisitor *v) override { v->visitSequenceNode(this); };

    std::vector<AstNode *> &getChildren() { return children; }

  private:
    std::vector<AstNode *> children = {};
};

class StatementNode : public AstNode {
  public:
    StatementNode() : AstNode(AstNode::STATEMENT) {}

    void accept(ASTVisitor *v) override { v->visitStatementNode(this); };

    bool isReturnStatement() { return this->returnStatement; }
    void setIsReturnStatement(bool returnStatement) { this->returnStatement = returnStatement; }

    AstNode *getChild() { return child; }
    void setChild(AstNode *child) { this->child = child; }

  private:
    bool returnStatement = false;
    AstNode *child = nullptr;
};

class AssignmentNode : public AstNode {
  public:
    explicit AssignmentNode() : AstNode(AstNode::ASSIGNMENT) {}

    void accept(ASTVisitor *v) override { v->visitAssignmentNode(this); };

    AstNode *getLeft() { return left; }
    void setLeft(AstNode *left) { this->left = left; }

    AstNode *getRight() { return right; }
    void setRight(AstNode *right) { this->right = right; }

  private:
    AstNode *left = nullptr;
    AstNode *right = nullptr;
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

    void accept(ASTVisitor *v) override { v->visitIntegerNode(this); };

    int getValue() { return value; }

  private:
    int value;
};

class FloatNode : public LiteralNode {
  public:
    explicit FloatNode(float value) : LiteralNode(LiteralNode::FLOAT), value(value) {}

    void accept(ASTVisitor *v) override { v->visitFloatNode(this); };

    float getValue() { return value; }

  private:
    float value;
};

class BoolNode : public LiteralNode {
  public:
    explicit BoolNode(bool value) : LiteralNode(LiteralNode::BOOL), value(value) {}

    void accept(ASTVisitor *v) override { v->visitBoolNode(this); };

    bool getValue() { return value; }

  private:
    bool value;
};

class VariableNode : public AstNode {
  public:
    explicit VariableNode(std::string name) : AstNode(AstNode::VARIABLE), name(std::move(name)) {}

    void accept(ASTVisitor *v) override { v->visitVariableNode(this); };

    std::string &getName() { return name; }

  private:
    std::string name;
};

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

class VariableDefinitionNode : public AstNode {
  public:
    explicit VariableDefinitionNode(std::string name, DataType type)
        : AstNode(AstNode::VARIABLE_DEFINITION), name(std::move(name)), type(type) {}

    void accept(ASTVisitor *v) override { v->visitVariableDefinitionNode(this); };

    std::string &getName() { return name; }

    DataType getType() { return type; }

  private:
    std::string name;
    DataType type;
};

class FunctionNode : public AstNode {
  public:
    explicit FunctionNode(std::string name, AstNode::DataType returnType)
        : AstNode(AstNode::FUNCTION), name(std::move(name)), returnType(returnType) {}

    void accept(ASTVisitor *v) override { v->visitFunctionNode(this); };

    AstNode *getBody() { return body; }
    void setBody(AstNode *body) { this->body = body; }

    std::vector<VariableDefinitionNode *> &getArguments() { return arguments; }

    std::string &getName() { return name; }

    AstNode::DataType getReturnType() { return returnType; }

    bool isExternal() { return body == nullptr; }

  private:
    std::string name;
    AstNode::DataType returnType;
    AstNode *body = nullptr;
    std::vector<VariableDefinitionNode *> arguments = {};
};

class CallNode : public AstNode {
  public:
    explicit CallNode(std::string name) : AstNode(AstNode::CALL), name(std::move(name)) {}

    void accept(ASTVisitor *v) override { v->visitCallNode(this); };

    std::string &getName() { return name; }

    std::vector<AstNode *> &getArguments() { return arguments; }

  private:
    std::string name;
    std::vector<AstNode *> arguments = {};
};

std::string to_string(AstNode::AstNodeType type);
