#include "IrGenerator.h"

#include <llvm/IR/Verifier.h>

#define ENABLE_OPTIMIZATIONS 0

llvm::Function *IrGenerator::getOrCreateStdLibFunction(const std::string &functionName) {
    auto *func = llvmModule.getFunction(functionName);
    if (func != nullptr) {
        return func;
    }
    if (functionName == "deleteString") {
        auto *stringType = getStringType();
        std::vector<llvm::Type *> arguments = {stringType->getPointerTo()};
        auto *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), arguments, false);
        return llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, functionName, llvmModule);
    }
    if (functionName == "createString") {
        auto *stringType = getStringType();
        std::vector<llvm::Type *> arguments = {
              llvm::PointerType::getInt8PtrTy(context),
              llvm::IntegerType::getInt64Ty(context),
              llvm::IntegerType::getInt64Ty(context),
        };
        auto *funcType = llvm::FunctionType::get(stringType->getPointerTo(), arguments, false);
        return llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, functionName, llvmModule);
    }
    if (functionName == "appendString") {
        auto *stringType = getStringType();
        std::vector<llvm::Type *> arguments = {
              stringType->getPointerTo(),
              stringType->getPointerTo(),
              stringType->getPointerTo(),
        };
        auto *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), arguments, false);
        return llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, functionName, llvmModule);
    }
    if (functionName == "assignString") {
        auto *stringType = getStringType();
        std::vector<llvm::Type *> arguments = {stringType->getPointerTo(), stringType->getPointerTo()};
        auto *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), arguments, false);
        return llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, functionName, llvmModule);
    }
    if (functionName == "malloc") {
        std::vector<llvm::Type *> arguments = {llvm::Type::getInt64Ty(context)};
        auto *funcType = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context), arguments, false);
        return llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, functionName, llvmModule);
    }
    if (functionName == "exit") {
        std::vector<llvm::Type *> arguments = {llvm::Type::getInt32Ty(context)};
        auto *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(context), arguments, false);
        return llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, functionName, llvmModule);
    }
    if (functionName == "memset") {
        std::vector<llvm::Type *> arguments = {
              llvm::Type::getInt8PtrTy(context),
              llvm::Type::getInt32Ty(context),
              llvm::Type::getInt64Ty(context),
        };
        auto *funcType = llvm::FunctionType::get(llvm::Type::getInt8PtrTy(context), arguments, false);
        return llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, functionName, llvmModule);
    }
    if (functionName == "printf") {
        std::vector<llvm::Type *> arguments = {
              llvm::Type::getInt8PtrTy(context), // format
        };
        auto *funcType = llvm::FunctionType::get(llvm::Type::getInt32Ty(context), arguments, true);
        return llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, functionName, llvmModule);
    }
    return nullptr;
}

llvm::Value *IrGenerator::createStdLibCall(const std::string &functionName, const std::vector<llvm::Value *> &args) {
    auto *func = getOrCreateStdLibFunction(functionName);
    if (func == nullptr) {
        logError("Could not create standard library call to function " + functionName);
        return nullptr;
    }

    return builder.CreateCall(func, args);
}

void IrGenerator::visitFunctionNode(FunctionNode *node) {
    log.debug("Enter Function");

    llvm::Function *previousFunction = currentFunction;
    bool previousGlobalScopeState = isGlobalScope;
    isGlobalScope = false;
    std::vector<FunctionArgument> arguments = {};
    for (const auto &arg : node->arguments) {
        FunctionArgument newArg = {arg->name, arg->type};
        arguments.push_back(newArg);
    }
    currentFunction = getOrCreateFunctionDefinition(node->name, node->returnType, arguments);

    if (!node->is_external()) {
        llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry-" + node->name, currentFunction);
        builder.SetInsertPoint(BB);

        withScope([this, &node]() {
            for (auto &arg : currentFunction->args()) {
                auto *value = createEntryBlockAlloca(arg.getType(), arg.getName().str());

                // store initial value
                builder.CreateStore(&arg, value);

                currentScope().definedVariables[arg.getName().str()] = value;
            }

            visitNode(node->body);

            if (node->returnType != ast::DataType(ast::SimpleDataType::VOID)) {
                // set insertion point to be before the return statement
                llvm::BasicBlock &lastBB = currentFunction->getBasicBlockList().back();
                llvm::BasicBlock::InstListType &instructionList = lastBB.getInstList();
                llvm::Instruction &returnInstruction = instructionList.back();
                builder.SetInsertPoint(&returnInstruction);
            }
        });
    }

    finalizeFunction(currentFunction, node->returnType, node->is_external());

    isGlobalScope = previousGlobalScopeState;
    currentFunction = previousFunction;
    // TODO(henne): this is a hack to enable us to get back to the previous functions insertion point
    //      we should save that last insertion point somewhere, instead of guessing it here
    builder.SetInsertPoint(&currentFunction->getBasicBlockList().back());

    log.debug("Exit Function");
}

llvm::Function *IrGenerator::getOrCreateFunctionDefinition(const std::string &name, const ast::DataType &returnType,
                                                           const std::vector<FunctionArgument> &arguments) {
    llvm::Function *function = llvmModule.getFunction(name);
    if (function != nullptr) {
        return function;
    }

    auto *retType = getType(returnType);

    std::vector<llvm::Type *> argumentTypes = {};
    argumentTypes.reserve(arguments.size());
    for (const auto &arg : arguments) {
        llvm::Type *type = getType(arg.type);
        argumentTypes.push_back(type);
    }

    llvm::FunctionType *functionType = llvm::FunctionType::get(retType, argumentTypes, false);

    function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, name, llvmModule);
    if (function == nullptr) {
        logError("Could not create function " + name);
        return nullptr;
    }

    unsigned int i = 0;
    for (auto &arg : function->args()) {
        arg.setName(arguments[i++].name);
    }

    return function;
}

llvm::Function *IrGenerator::getOrCreateFunctionDefinition(const FunctionSignature &signature) {
    return getOrCreateFunctionDefinition(signature.name, signature.returnType, signature.arguments);
}

void IrGenerator::finalizeFunction(llvm::Function *function, const ast::DataType &returnType,
                                   const bool isExternalFunction) {
    if (!isExternalFunction && returnType == ast::DataType(ast::SimpleDataType::VOID)) {
        builder.SetInsertPoint(&function->getBasicBlockList().back());
        builder.CreateRetVoid();
    }

    if (llvm::verifyFunction(*function, &llvm::errs())) {
        return;
    }

    //    function->viewCFG();

    // FIXME create command line option to switch optimizations on/off
#if ENABLE_OPTIMIZATIONS
    if (!isExternalFunction) {
        bool verbose = false;
        llvm::LoopAnalysisManager loopAnalysisManager(verbose);
        llvm::FunctionAnalysisManager functionAnalysisManager(verbose);
        llvm::CGSCCAnalysisManager cgsccAnalysisManager(verbose);
        llvm::ModuleAnalysisManager moduleAnalysisManager(verbose);
        llvm::PassBuilder passBuilder;
        passBuilder.registerModuleAnalyses(moduleAnalysisManager);
        passBuilder.registerCGSCCAnalyses(cgsccAnalysisManager);
        passBuilder.registerFunctionAnalyses(functionAnalysisManager);
        passBuilder.registerLoopAnalyses(loopAnalysisManager);
        passBuilder.crossRegisterProxies(loopAnalysisManager, functionAnalysisManager, cgsccAnalysisManager,
                                         moduleAnalysisManager);
        llvm::FunctionPassManager functionPassManager = passBuilder.buildFunctionSimplificationPipeline(
              llvm::PassBuilder::OptimizationLevel::O0, llvm::PassBuilder::ThinLTOPhase::None, verbose);
        functionPassManager.run(*function, functionAnalysisManager);
    }
#endif
}

void IrGenerator::visitCallNode(CallNode *node) {
    log.debug("Enter Function Call");

    llvm::Function *calleeFunc = llvmModule.getFunction(node->name);
    if (calleeFunc == nullptr) {
        const FunctionResolveResult resolveResult = functionResolver.resolveFunction(module, node->name);
        if (!resolveResult.functionExists) {
            return logError("Undefined function '" + node->name + "'");
        }

        calleeFunc = getOrCreateFunctionDefinition(resolveResult.signature);
        if (calleeFunc == nullptr) {
            return logError("Could not generate external definition for function '" + node->name + "'");
        }
    }

    if (calleeFunc->arg_size() != node->arguments.size()) {
        return logError("Wrong number of arguments passed.");
    }

    std::vector<llvm::Value *> arguments;
    for (auto &argument : node->arguments) {
        visitNode(argument);
        auto itr = nodesToValues.find(argument);
        if (itr == nodesToValues.end()) {
            return logError("Could not generate code for argument.");
        }
        if (typeResolver.getTypeOf(module, argument) == ast::DataType(ast::SimpleDataType::STRING)) {
            arguments.push_back(builder.CreateLoad(itr->second));
        } else {
            arguments.push_back(itr->second);
        }
    }

    llvm::Value *call = nullptr;
    if (calleeFunc->getReturnType()->getTypeID() == llvm::Type::TypeID::VoidTyID) {
        call = builder.CreateCall(calleeFunc, arguments);
    } else {
        call = builder.CreateCall(calleeFunc, arguments, "call");
    }
    nodesToValues[AST_NODE(node)] = call;

    log.debug("Exit Function Call");
}

bool IrGenerator::isPrimitiveType(const ast::DataType &type) {
    return type == ast::DataType(ast::SimpleDataType::BOOLEAN) || type == ast::DataType(ast::SimpleDataType::INTEGER) ||
           type == ast::DataType(ast::SimpleDataType::FLOAT);
}
