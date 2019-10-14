#include "IRGenerator.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>

#include <iostream>

#include "../Utils.h"

IRGenerator::IRGenerator(const Program &program, const bool verbose)
    : program(program), verbose(verbose), builder(context), module(program.fileName, context) {}

void IRGenerator::logError(const std::string &msg) { std::cerr << msg << std::endl; }

auto IRGenerator::getType(ast::DataType type) -> llvm::Type * {
    switch (type) {
    case ast::DataType::VOID:
        return llvm::Type::getVoidTy(context);
    case ast::DataType::INT:
        return llvm::Type::getInt32Ty(context);
    case ast::DataType::FLOAT:
        return llvm::Type::getFloatTy(context);
    case ast::DataType::BOOL:
        return llvm::Type::getInt1Ty(context);
    default:
        return nullptr;
    }
}

auto IRGenerator::createEntryBlockAlloca(llvm::Type *type, const std::string &name) -> llvm::AllocaInst * {
    llvm::BasicBlock *block = builder.GetInsertBlock();
    if (block == nullptr) {
        return nullptr;
    }
    auto function = block->getParent();
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, nullptr, name);
}

auto IRGenerator::getInitializer(const ast::DataType &dt) -> llvm::Constant * {
    llvm::Type *ty = getType(dt);
    switch (dt) {
    case ast::DataType::FLOAT:
        return llvm::ConstantFP::get(ty, 0);
    case ast::DataType::INT:
    case ast::DataType::BOOL:
        return llvm::ConstantInt::get(ty, 0);
    case ast::DataType::VOID:
    default:
        return nullptr;
    }
}

void IRGenerator::setupGlobalInitialization(llvm::Function *func) {
    std::vector<llvm::Type *> types = {llvm::Type::getInt32Ty(context), func->getType(),
                                       llvm::PointerType::getInt8PtrTy(context)};
    auto structType = llvm::StructType::get(context, types);
    module.getOrInsertGlobal("llvm.global_ctors", llvm::ArrayType::get(structType, 1));
    llvm::GlobalVariable *ctorsVar = module.getGlobalVariable("llvm.global_ctors");
    ctorsVar->setLinkage(llvm::GlobalValue::LinkageTypes::AppendingLinkage);

    llvm::ConstantInt *intValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 65535);
    llvm::ConstantPointerNull *nullValue = llvm::ConstantPointerNull::get(llvm::PointerType::getInt8PtrTy(context));
    std::vector<llvm::Constant *> structValues = {intValue, func, nullValue};
    std::vector<llvm::Constant *> arrayValues = {llvm::ConstantStruct::get(structType, structValues)};
    llvm::Constant *initializer = llvm::ConstantArray::get(llvm::ArrayType::get(structType, 1), arrayValues);
    ctorsVar->setInitializer(initializer);
}

void IRGenerator::visitSequenceNode(SequenceNode *node) {
    LOG("Enter Sequence")

    llvm::Function *initFunc = nullptr;
    if (currentFunction == nullptr) {
        // TODO(henne): make sure this function name does not collide with any user defined functions
        initFunc = getOrCreateFunctionDefinition("__ctor", ast::DataType::VOID, {});

        llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry-ctor", initFunc);
        builder.SetInsertPoint(BB);

        currentFunction = initFunc;
        isGlobalScope = true;
    }

    for (auto child : node->getChildren()) {
        child->accept(this);
    }

    if (!node->getChildren().empty()) {
        nodesToValues[node] = nodesToValues[node->getChildren().back()];
    }

    if (initFunc != nullptr) {
        finalizeFunction(initFunc, ast::DataType::VOID, false);
        // TODO(henne): henne: don't generate global init function, if there are no globals
        setupGlobalInitialization(initFunc);
        isGlobalScope = false;
    }

    LOG("Exit Sequence")
}

void IRGenerator::visitStatementNode(StatementNode *node) {
    LOG("Enter Statement")

    if (node->getChild() == nullptr) {
        return;
    }

    node->getChild()->accept(this);
    auto value = nodesToValues[node->getChild()];
    if (node->isReturnStatement()) {
        builder.CreateRet(value);
    }
    nodesToValues[node] = value;

    LOG("Exit Statement")
}

void IRGenerator::visitIfStatementNode(IfStatementNode *node) {
    LOG("Enter IfStatement")

    node->getCondition()->accept(this);
    auto condition = nodesToValues[node->getCondition()];
    llvm::Function *function = builder.GetInsertBlock()->getParent();

    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then", function);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(context, "else");
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "if_merge");

    builder.CreateCondBr(condition, thenBB, elseBB);

    builder.SetInsertPoint(thenBB);
    if (node->getIfBody() != nullptr) {
        node->getIfBody()->accept(this);
        // TODO(henne): henne: Make sure that CreateBr is not called if we emitted a return statement at the end of the if-body.
        //  We need scoping, for this to work.
    }
    builder.CreateBr(mergeBB);

    function->getBasicBlockList().push_back(elseBB);
    builder.SetInsertPoint(elseBB);

    if (node->getElseBody() != nullptr) {
        node->getElseBody()->accept(this);
        // TODO(henne): henne: Make sure that CreateBr is not called if we emitted a return statement at the end of the if-body.
        //  We need scoping, for this to work.
    }
    builder.CreateBr(mergeBB);

    function->getBasicBlockList().push_back(mergeBB);
    builder.SetInsertPoint(mergeBB);

    LOG("Exit IfStatement")
}

void IRGenerator::print(const bool writeToFile) {
    std::string fileName = program.fileName + ".llvm";
    std::error_code EC;
    llvm::raw_fd_ostream dest(fileName, EC, llvm::sys::fs::OF_None);
    module.print(llvm::outs(), nullptr);
    if (writeToFile) {
        module.print(dest, nullptr);
    }
}

void IRGenerator::run(AstNode *root) {
    if (root == nullptr) {
        return;
    }

    root->accept(this);

    this->print();
}
