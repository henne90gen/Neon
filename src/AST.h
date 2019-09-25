#pragma once

#include <utility>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include "Parser.h"

class AstNode {
  public:
    enum AstNodeType {
        SEQUENCE,
        STATEMENT,
        LITERAL,
        UNARY_OPERATION,
        BINARY_OPERATION,
        FUNCTION,
        VARIABLE_DEFINITION,
        VARIABLE
    };
    enum DataType { NONE, INT, FLOAT, BOOL };

    explicit AstNode(AstNodeType type) : type(type) {}

    virtual void print(int indentation = 0) = 0;

    virtual llvm::Value *generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) = 0;

    AstNodeType getAstNodeType() { return type; }

  private:
    AstNodeType type;
};

class SequenceNode : public AstNode {
  public:
    SequenceNode() : AstNode(AstNode::SEQUENCE) {}

    void print(int indentation) override;

    llvm::Value *generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) override;

    std::vector<AstNode *> &getChildren() { return children; }

  private:
    std::vector<AstNode *> children = {};
};

class StatementNode : public AstNode {
  public:
    StatementNode() : AstNode(AstNode::STATEMENT) {}

    void print(int indentation) override;

    llvm::Value *generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) override;

    void setIsReturnStatement(bool isReturnStatement) { this->isReturnStatement = isReturnStatement; }

    AstNode *getChild() { return child; }
    void setChild(AstNode *child) { this->child = child; }

  private:
    bool isReturnStatement = false;
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

    llvm::Value *generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) override;

  private:
    int value;
};

class FloatNode : public LiteralNode {
  public:
    explicit FloatNode(float value) : LiteralNode(LiteralNode::FLOAT), value(value) {}

    float getValue() { return value; }

    void print(int indentation) override;

    llvm::Value *generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) override;

  private:
    float value;
};

class BoolNode : public LiteralNode {
  public:
    explicit BoolNode(bool value) : LiteralNode(LiteralNode::BOOL), value(value) {}

    bool getValue() { return value; }

    void print(int indentation) override;

    llvm::Value *generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) override;

  private:
    bool value;
};

class VariableNode : public AstNode {
  public:
    explicit VariableNode(std::string name) : AstNode(AstNode::VARIABLE), name(std::move(name)) {}

    void print(int indentation) override;

    llvm::Value *generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) override;

  private:
    std::string name;
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

    llvm::Value *generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) override;

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

    llvm::Value *generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) override;

  private:
    BinaryOperationType type;
    AstNode *left = nullptr;
    AstNode *right = nullptr;
};

class VariableDefinitionNode : public AstNode {
  public:
    explicit VariableDefinitionNode(std::string name, DataType type)
        : AstNode(AstNode::VARIABLE_DEFINITION), name(std::move(name)), type(type) {}

    void print(int indentation) override;

    llvm::Value *generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) override;

  private:
    std::string name;
    DataType type;
};

class FunctionNode : public AstNode {
  public:
    explicit FunctionNode(std::string name, AstNode::DataType returnType)
        : AstNode(AstNode::FUNCTION), name(std::move(name)), returnType(returnType) {}

    void print(int indentation) override;

    llvm::Value *generateIR(llvm::LLVMContext &context, llvm::IRBuilder<> &builder, llvm::Module &module) override;

    void setBody(AstNode *body) { this->body = body; }

    std::vector<VariableDefinitionNode *> &getArguments() { return arguments; }

    std::string &getName() { return name; }

  private:
    std::string name;
    AstNode::DataType returnType;
    AstNode *body = nullptr;
    std::vector<VariableDefinitionNode *> arguments = {};
};

AstNode *createAstFromParseTree(ParseTreeNode *node);

void generateIR(AstNode *root);
