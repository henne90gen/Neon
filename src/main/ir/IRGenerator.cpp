#include "IRGenerator.h"

#include <iostream>
#include <llvm/Support/FileSystem.h>

IRGenerator::IRGenerator(const Program &program, const bool verbose = false)
    : program(program), verbose(verbose), builder(context), module(program.fileName, context) {}

void IRGenerator::logError(const std::string &msg) { std::cerr << msg << std::endl; }

llvm::Type *IRGenerator::getType(AstNode::DataType type) {
    switch (type) {
    case AstNode::VOID:
        return llvm::Type::getVoidTy(context);
    case AstNode::INT:
        return llvm::Type::getInt32Ty(context);
    case AstNode::FLOAT:
        return llvm::Type::getFloatTy(context);
    case AstNode::BOOL:
        return llvm::Type::getInt1Ty(context);
    default:
        return nullptr;
    }
}

llvm::AllocaInst *IRGenerator::createEntryBlockAlloca(llvm::Type *type, const std::string &name) {
    llvm::BasicBlock *block = builder.GetInsertBlock();
    if (block == nullptr) {
        return nullptr;
    }
    auto function = block->getParent();
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, nullptr, name);
}

llvm::Constant *IRGenerator::getInitializer(const AstNode::DataType &dt) {
    llvm::Type *ty = getType(dt);
    switch (dt) {
    case AstNode::FLOAT:
        return llvm::ConstantFP::get(ty, 0);
    case AstNode::INT:
    case AstNode::BOOL:
        return llvm::ConstantInt::get(ty, 0);
    case AstNode::VOID:
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
        // TODO make sure this function name does not collide with any user defined functions
        initFunc = getOrCreateFunctionDefinition("__ctor", AstNode::DataType::VOID, {});

        llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry-ctor", initFunc);
        builder.SetInsertPoint(BB);

        currentFunction = initFunc;
        isGlobalScope = true;
    }

    for (auto child : node->getChildren()) {
        child->accept(this);
    }

    if (initFunc != nullptr) {
        finalizeFunction(initFunc, AstNode::DataType::VOID, false);
        // TODO don't generate global init function, if there are no globals
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
    if (node->isReturnStatement()) {
        builder.CreateRet(nodesToValues[node->getChild()]);
    }

    LOG("Exit Statement")
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
