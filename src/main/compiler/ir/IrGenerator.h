#pragma once

#include "../../Module.h"
#include "../FunctionResolver.h"
#include "../TypeResolver.h"
#include "../ast/AstVisitor.h"
#include "../ast/nodes/AllNodes.h"
#include "Scope.h"

#include <iostream>
#include <unordered_map>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#define LOG(msg)                                                                                                       \
    if (verbose) {                                                                                                     \
        std::cout << msg << std::endl;                                                                                 \
    }

class IrGenerator : public AstVisitor {
  public:
    explicit IrGenerator(Module *module, FunctionResolver &functionResolver, TypeResolver &typeResolver, bool verbose);

    void visitAssignmentNode(AssignmentNode *node) override;
    void visitBinaryOperationNode(BinaryOperationNode *node) override;
    void visitBoolNode(BoolNode *node) override;
    void visitCallNode(CallNode *node) override;
    void visitFloatNode(FloatNode *node) override;
    void visitForStatementNode(ForStatementNode *node) override;
    void visitFunctionNode(FunctionNode *node) override;
    void visitIfStatementNode(IfStatementNode *node) override;
    void visitIntegerNode(IntegerNode *node) override;
    void visitSequenceNode(SequenceNode *node) override;
    void visitStatementNode(StatementNode *node) override;
    void visitStringNode(StringNode *node) override;
    void visitUnaryOperationNode(UnaryOperationNode *node) override;
    void visitVariableNode(VariableNode *node) override;
    void visitVariableDefinitionNode(VariableDefinitionNode *node) override;

    void run();

    void writeToFile();

  private:
    Module *module;
    FunctionResolver &functionResolver;
    TypeResolver &typeResolver;
    const bool verbose = false;

    llvm::LLVMContext &context;
    llvm::Module &llvmModule;
    llvm::IRBuilder<> builder;

    std::unordered_map<std::string, int> metrics = {};
    std::vector<std::string> errors = {};

    llvm::Function *currentFunction = nullptr;
    bool isGlobalScope = false;
    std::unordered_map<AstNode *, llvm::Value *> nodesToValues = {};
    std::vector<Scope> scopeStack = {};

    // This is used to save a pointer to write to (for structs)
    llvm::Value* currentDestination = nullptr;

    llvm::Value *findVariable(const std::string &name);
    Scope &currentScope();
    void pushScope();
    void popScope();
    void withScope(const std::function<void(void)> &func);

    void printMetrics();
    void printErrors();

    void logError(const std::string &msg);
    llvm::Type *getType(ast::DataType type);
    llvm::Function *getOrCreateFunctionDefinition(const std::string &name, const ast::DataType returnType,
                                                  const std::vector<FunctionArgument> &arguments);
    llvm::Function *getOrCreateFunctionDefinition(const FunctionSignature &signature);
    llvm::AllocaInst *createEntryBlockAlloca(llvm::Type *type, const std::string &name);
    void finalizeFunction(llvm::Function *function, ast::DataType returnType, bool isExternalFunction);
    llvm::Constant *getInitializer(const ast::DataType &dt, bool isArray, unsigned int arraySize);
    void setupGlobalInitialization(llvm::Function *func);

    void emitIntegerDivision(BinaryOperationNode *node);
    void emitIntegerOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r);
    void emitFloatDivision(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r);

    llvm::StructType *getStringType();
    static bool isPrimitiveType(ast::DataType type);
};
