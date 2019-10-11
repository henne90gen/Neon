#pragma once

#include "../Program.h"
#include "../ast/ASTVisitor.h"
#include "../ast/nodes/AllNodes.h"

#include <iostream>
#include <unordered_map>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#define LOG(msg)                                                                                                       \
    if (verbose) {                                                                                                     \
        std::cout << msg << std::endl;                                                                                 \
    }

class IRGenerator : public ASTVisitor {
  public:
    explicit IRGenerator(const Program &program, bool verbose);

    void visitFunctionNode(FunctionNode *node) override;
    void visitCallNode(CallNode *node) override;
    void visitIfStatementNode(IfStatementNode *node) override;
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

    void run(AstNode *root);

    void print(bool writeToFile = true);

    llvm::Module &getModule() { return module; }

  private:
    const Program &program;
    const bool verbose = false;
    llvm::LLVMContext context = {};
    llvm::IRBuilder<> builder;
    llvm::Module module;

    llvm::Function *currentFunction = nullptr;
    bool isGlobalScope = false;
    std::unordered_map<AstNode *, llvm::Value *> nodesToValues = {};
    std::unordered_map<std::string, llvm::Value *> definedVariables = {};

    static void logError(const std::string &msg);
    llvm::Type *getType(ast::DataType type);
    llvm::Function *getOrCreateFunctionDefinition(const std::string &name, ast::DataType returnType,
                                                  const std::vector<VariableDefinitionNode *> &arguments);
    llvm::AllocaInst *createEntryBlockAlloca(llvm::Type *type, const std::string &name);
    void finalizeFunction(llvm::Function *function, ast::DataType returnType, bool isExternalFunction);
    llvm::Constant *getInitializer(const ast::DataType &dt);
    void setupGlobalInitialization(llvm::Function *func);
};
