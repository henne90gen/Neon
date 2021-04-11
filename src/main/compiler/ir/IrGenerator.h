#pragma once

#include "../../BuildEnv.h"
#include "../../Module.h"
#include "../FunctionResolver.h"
#include "../TypeResolver.h"
#include "../ast/AstNode.h"
#include "Scope.h"

#include <iostream>
#include <unordered_map>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

class IrGenerator {
  public:
    explicit IrGenerator(const BuildEnv *buildEnv, Module *module, FunctionResolver &functionResolver,
                         TypeResolver &typeResolver, const Logger &logger);

    void visitAssertNode(AssertNode *node);
    void visitAssignmentNode(AssignmentNode *node);
    void visitBinaryOperationNode(BinaryOperationNode *node);
    void visitCallNode(CallNode *node);
    void visitForStatementNode(ForStatementNode *node);
    void visitFunctionNode(FunctionNode *node);
    void visitIfStatementNode(IfStatementNode *node);
    void visitMemberAccessNode(MemberAccessNode *node);
    void visitSequenceNode(SequenceNode *node);
    void visitStatementNode(StatementNode *node);
    void visitTypeDeclarationNode(TypeDeclarationNode *node);
    void visitStringNode(LiteralNode *node);
    void visitUnaryOperationNode(UnaryOperationNode *node);
    void visitVariableNode(VariableNode *node);
    void visitVariableDefinitionNode(VariableDefinitionNode *node);

    void run();

    void writeToFile();

  private:
    const BuildEnv *buildEnv;
    Module *module;
    FunctionResolver &functionResolver;
    TypeResolver &typeResolver;
    const Logger &log;

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
    llvm::Value *currentDestination = nullptr;

    llvm::Value *findVariable(const std::string &name);
    Scope &currentScope();
    void pushScope();
    void popScope();
    void withScope(const std::function<void(void)> &func);

    void printMetrics();
    void printErrors();

    void logError(const std::string &msg);
    llvm::Type *getType(const ast::DataType &type);
    llvm::Function *getOrCreateFunctionDefinition(const std::string &name, const ast::DataType &returnType,
                                                  const std::vector<FunctionArgument> &arguments);
    llvm::Function *getOrCreateFunctionDefinition(const FunctionSignature &signature);
    llvm::StructType *getOrCreateComplexType(const ComplexType &type);
    llvm::AllocaInst *createEntryBlockAlloca(llvm::Type *type, const std::string &name);
    void finalizeFunction(llvm::Function *function, const ast::DataType &returnType, bool isExternalFunction);
    llvm::Constant *getInitializer(const ast::DataType &dt, bool isArray, unsigned int arraySize);
    void setupGlobalInitialization(llvm::Function *func);

    void emitIntegerOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r);
    void emitFloatOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r);
    void emitStringOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r);
    void emitBooleanOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r);

    llvm::StructType *getStringType();
    static bool isPrimitiveType(const ast::DataType &type);

    llvm::Function *getOrCreateStdLibFunction(const std::string &functionName);
    llvm::Value *createStdLibCall(const std::string &functionName, const std::vector<llvm::Value *> &args);

    std::string getTypeFormatSpecifier(AstNode *node);
    void visitNode(AstNode *node);
    void visitLiteralNode(LiteralNode *node);
};
