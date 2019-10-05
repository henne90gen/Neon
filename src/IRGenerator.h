#pragma once

#include "Program.h"
#include "ast/AST.h"

#include <unordered_map>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Transforms/Utils/CtorUtils.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"

class IRGenerator : public ASTVisitor {
  public:
    explicit IRGenerator(const Program &program, bool verbose);

    void visitFunctionNode(FunctionNode *node) override;
    void visitCallNode(CallNode *node) override;
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

    llvm::Type *getType(AstNode::DataType type);
    llvm::Function *getOrCreateFunctionDefinition(const std::string &name, AstNode::DataType returnType,
                                        const std::vector<VariableDefinitionNode *> &arguments);
    llvm::AllocaInst *createEntryBlockAlloca(llvm::Type *type, const std::string &name);
    void finalizeFunction(llvm::Function *function, const AstNode::DataType returnType, const bool isExternalFunction);
    llvm::Constant *getInitializer(const AstNode::DataType &dt);
    void setupGlobalInitialization(llvm::Function *func);
};
