#pragma once

#include "ast/AST.h"

#include <unordered_map>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_ostream.h"

class IRGenerator : public ASTVisitor {
  public:
    IRGenerator() : builder(context), module("MyModuleName", context) {}

    void visitFunctionNode(FunctionNode *node) override;
    void visitVariableNode(VariableNode *node) override;
    void visitVariableDefinitionNode(VariableDefinitionNode *node) override;
    void visitBinaryOperationNode(BinaryOperationNode *node) override;
    void visitUnaryOperationNode(UnaryOperationNode *node) override;
    void visitAssignmentNode(AssignmentNode *node) override;
    void visitSequenceNode(SequenceNode *node) override;
    void visitStatementNode(StatementNode *node) override;
    void visitFloatNode(FloatNode *node) override;
    void visitIntegerNode(IntegerNode *node) override;
    void visitBoolNode(BoolNode *node) override;

    void print();

  private:
    llvm::LLVMContext context = {};
    llvm::IRBuilder<> builder;
    llvm::Module module;

    llvm::Function *currentFunction = nullptr;
    std::unordered_map<AstNode *, llvm::Value *> nodesToValues = {};
    std::unordered_map<std::string, llvm::AllocaInst *> definedVariables = {};

    void logError(const std::string &msg);
    llvm::Type *getType(AstNode::DataType type);
    llvm::AllocaInst *createEntryBlockAlloca(llvm::Type *type, const std::string &name);
};

void generateIR(AstNode *root);
