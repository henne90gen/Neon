#pragma once

#include "../../BuildEnv.h"
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

class IrGenerator : public AstVisitor {
  public:
    explicit IrGenerator(const BuildEnv *buildEnv, Module *module, FunctionResolver &functionResolver,
                         TypeResolver &typeResolver, const Logger &logger);

    void visitAssertNode(AssertNode *node) override;
    void visitAssignmentNode(AssignmentNode *node) override;
    void visitBinaryOperationNode(BinaryOperationNode *node) override;
    void visitBoolNode(BoolNode *node) override;
    void visitCallNode(CallNode *node) override;
    void visitFloatNode(FloatNode *node) override;
    void visitForStatementNode(ForStatementNode *node) override;
    void visitFunctionNode(FunctionNode *node) override;
    void visitIfStatementNode(IfStatementNode *node) override;
    void visitIntegerNode(IntegerNode *node) override;
    void visitMemberAccessNode(MemberAccessNode *node) override;
    void visitSequenceNode(SequenceNode *node) override;
    void visitStatementNode(StatementNode *node) override;
    void visitTypeDeclarationNode(TypeDeclarationNode *node) override;
    void visitStringNode(StringNode *node) override;
    void visitUnaryOperationNode(UnaryOperationNode *node) override;
    void visitVariableNode(VariableNode *node) override;
    void visitVariableDefinitionNode(VariableDefinitionNode *node) override;

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

    llvm::StructType *getStringType();
    static bool isPrimitiveType(const ast::DataType &type);

    llvm::Function *getOrCreateStdLibFunction(const std::string &functionName);
    llvm::Value *createStdLibCall(const std::string &functionName, const std::vector<llvm::Value *> &args);

    std::string getTypeFormatSpecifier(AstNode *node);
};
