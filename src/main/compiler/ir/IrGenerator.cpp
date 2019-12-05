#include "IrGenerator.h"

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
#include <llvm/Transforms/Utils/Cloning.h>

#include <iostream>

#include "../../Utils.h"

IrGenerator::IrGenerator(Module *module, const bool verbose)
    : module(module), verbose(verbose), context(module->llvmModule.getContext()), llvmModule(module->llvmModule),
      builder(context) {
    pushScope();
}

void IrGenerator::logError(const std::string &msg) { errors.push_back(msg); }

llvm::Type *IrGenerator::getType(ast::DataType type) {
    switch (type) {
    case ast::DataType::VOID:
        return llvm::Type::getVoidTy(context);
    case ast::DataType::INT:
        return llvm::Type::getInt64Ty(context);
    case ast::DataType::FLOAT:
        return llvm::Type::getDoubleTy(context);
    case ast::DataType::BOOL:
        return llvm::Type::getInt1Ty(context);
    default:
        return nullptr;
    }
}

llvm::AllocaInst *IrGenerator::createEntryBlockAlloca(llvm::Type *type, const std::string &name) {
    llvm::BasicBlock *block = builder.GetInsertBlock();
    if (block == nullptr) {
        return nullptr;
    }
    auto function = block->getParent();
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, nullptr, name);
}

llvm::Constant *IrGenerator::getInitializer(const ast::DataType &dt, bool isArray, unsigned int arraySize) {
    // TODO(henne): refactor this method once we have a solid type system
    if (isArray) {
        llvm::ArrayType *ty = llvm::ArrayType::get(getType(dt), arraySize);
        return llvm::ConstantAggregateZero::get(ty);
    }
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

void IrGenerator::setupGlobalInitialization(llvm::Function *func) {
    std::vector<llvm::Type *> types = {llvm::Type::getInt32Ty(context), func->getType(),
                                       llvm::PointerType::getInt8PtrTy(context)};
    auto structType = llvm::StructType::get(context, types);
    llvmModule.getOrInsertGlobal("llvm.global_ctors", llvm::ArrayType::get(structType, 1));
    llvm::GlobalVariable *ctorsVar = llvmModule.getGlobalVariable("llvm.global_ctors");
    ctorsVar->setLinkage(llvm::GlobalValue::LinkageTypes::AppendingLinkage);

    const int globalInitializerNumber = 65535;
    llvm::ConstantInt *intValue = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), globalInitializerNumber);
    llvm::ConstantPointerNull *nullValue = llvm::ConstantPointerNull::get(llvm::PointerType::getInt8PtrTy(context));
    std::vector<llvm::Constant *> structValues = {intValue, func, nullValue};
    std::vector<llvm::Constant *> arrayValues = {llvm::ConstantStruct::get(structType, structValues)};
    llvm::Constant *initializer = llvm::ConstantArray::get(llvm::ArrayType::get(structType, 1), arrayValues);
    ctorsVar->setInitializer(initializer);
}

void IrGenerator::visitSequenceNode(SequenceNode *node) {
    LOG("Enter Sequence")

    llvm::Function *initFunc = nullptr;
    if (currentFunction == nullptr) {
        // TODO(henne): make sure this function name does not collide with any user defined functions
        initFunc = getOrCreateFunctionDefinition("global-ctor-" + module->fileName, ast::DataType::VOID, {});

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

void IrGenerator::print(const bool writeToFile) {
    std::string fileName = module->fileName + ".llvm";
    std::error_code EC;
    llvmModule.print(llvm::outs(), nullptr);
    if (writeToFile) {
        llvm::raw_fd_ostream dest(fileName, EC, llvm::sys::fs::OF_None);
        llvmModule.print(dest, nullptr);
        dest.flush();
        dest.close();
    }
}

void IrGenerator::run() {
    if (module->root == nullptr) {
        return;
    }

    module->root->accept(this);

    this->printMetrics();
    if (!errors.empty()) {
        printErrors();
        exit(1);
    } else {
        this->print();
    }
}

llvm::Value *IrGenerator::findVariable(const std::string &name) {
    metrics["variableLookups"]++;

    unsigned long currentScope = definedVariables.size() - 1;
    while (currentScope >= 0) {
        auto &scope = definedVariables[currentScope];
        auto result = scope.find(name);
        if (result != scope.end()) {

            metrics["variableLookupsSuccessful"]++;

            return result->second;
        }
        currentScope--;
    }

    metrics["variableLookupsFailure"]++;

    return nullptr;
}

std::unordered_map<std::string, llvm::Value *> &IrGenerator::currentScope() {
    return definedVariables[definedVariables.size() - 1];
}

void IrGenerator::pushScope() { definedVariables.emplace_back(); }

void IrGenerator::popScope() { definedVariables.pop_back(); }

void IrGenerator::withScope(const std::function<void(void)> &func) {
    pushScope();
    func();
    popScope();
}

void IrGenerator::printMetrics() {
    for (const auto &metric : metrics) {
        std::cout << metric.first << ": " << metric.second << std::endl;
    }
}

void IrGenerator::printErrors() {
    std::cerr << std::endl;
    std::cerr << "The following errors occured:" << std::endl;
    for (const auto &msg : errors) {
        std::cerr << "\t" << msg << std::endl;
    }
    std::cerr << std::endl;
}
