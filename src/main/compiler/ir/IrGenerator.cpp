#include "IrGenerator.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include <iostream>

#include "../../Utils.h"

IrGenerator::IrGenerator(const BuildEnv *buildEnv, Module *module, FunctionResolver &functionResolver,
                         TypeResolver &typeResolver, const Logger &logger)
    : buildEnv(buildEnv), module(module), functionResolver(functionResolver), typeResolver(typeResolver), log(logger),
      context(module->llvmModule.getContext()), llvmModule(module->llvmModule), builder(context) {
    pushScope();
}

void IrGenerator::logError(const std::string &msg) { errors.push_back(msg); }

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
    // TODO(henne): refactor this method once we have a "solid" type system
    if (isArray) {
        llvm::ArrayType *ty = llvm::ArrayType::get(getType(dt), arraySize);
        return llvm::ConstantAggregateZero::get(ty);
    }
    if (ast::isSimpleDataType(dt)) {
        llvm::Type *ty = getType(dt);
        auto simpleType = ast::toSimpleDataType(dt);
        switch (simpleType) {
        case ast::SimpleDataType::FLOAT:
            return llvm::ConstantFP::get(ty, 0);
        case ast::SimpleDataType::INT:
        case ast::SimpleDataType::BOOL:
            return llvm::ConstantInt::get(ty, 0);
        case ast::SimpleDataType::VOID:
        default:
            return nullptr;
        }
    } else {
        // TODO get or create the correct complex llvm type
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
    log.debug("Enter Sequence");

    llvm::Function *initFunc = nullptr;
    if (currentFunction == nullptr) {
        // TODO(henne): make sure this function name does not collide with any user defined functions
        // TODO(henne): maybe don't use the full filepath...
        initFunc = getOrCreateFunctionDefinition("global-ctor-" + module->getFilePath().string(),
                                                 ast::DataType(ast::SimpleDataType::VOID), {});

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
        finalizeFunction(initFunc, ast::DataType(ast::SimpleDataType::VOID), false);
        // TODO(henne): don't generate global init function, if there are no globals
        setupGlobalInitialization(initFunc);
        isGlobalScope = false;
    }

    log.debug("Exit Sequence");
}

void IrGenerator::writeToFile() {
    std::string filePath = buildEnv->buildDirectory + module->getFilePath().string() + ".llvm";
    std::error_code EC;
    llvm::raw_fd_ostream dest(filePath, EC, llvm::sys::fs::OF_None);
    if (EC) {
        llvm::errs() << "Failed to open file (" << filePath << "): " << EC.message() << "\n";
        exit(1);
    }
    llvmModule.print(dest, nullptr);
    dest.flush();
    dest.close();
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
        this->writeToFile();
        if (log.getLogLevel() == Logger::LogLevel::DEBUG_) {
            llvmModule.print(llvm::outs(), nullptr);
        }
    }
}

llvm::Value *IrGenerator::findVariable(const std::string &name) {
    metrics["variableLookups"]++;

    unsigned long currentScope = scopeStack.size() - 1;
    while (currentScope >= 0) {
        auto &scope = scopeStack[currentScope];
        auto result = scope.definedVariables.find(name);

        if (result != scope.definedVariables.end()) {
            metrics["variableLookupsSuccessful"]++;
            return result->second;
        }

        currentScope--;
    }

    metrics["variableLookupsFailure"]++;

    return nullptr;
}

Scope &IrGenerator::currentScope() { return scopeStack[scopeStack.size() - 1]; }

void IrGenerator::pushScope() { scopeStack.emplace_back(); }

void IrGenerator::popScope() {
    auto currentScope = scopeStack.back();
    for (auto &func : currentScope.cleanUpFunctions) {
        func();
    }
    scopeStack.pop_back();
}

void IrGenerator::withScope(const std::function<void(void)> &func) {
    pushScope();
    func();
    popScope();
}

void IrGenerator::printMetrics() {
    if (log.getLogLevel() != Logger::LogLevel::DEBUG_) {
        return;
    }

    for (const auto &metric : metrics) {
        log.debug(metric.first + ": " + std::to_string(metric.second));
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
