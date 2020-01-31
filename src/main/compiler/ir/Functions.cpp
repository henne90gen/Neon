#include "IrGenerator.h"

#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>

#define ENABLE_OPTIMIZATIONS 0

void IrGenerator::visitFunctionNode(FunctionNode *node) {
    LOG("Enter Function")

    llvm::Function *previousFunction = currentFunction;
    bool previousGlobalScopeState = isGlobalScope;
    isGlobalScope = false;
    std::vector<FunctionArgument> arguments = {};
    for (const auto &arg : node->getArguments()) {
        FunctionArgument newArg = {arg->getName(), arg->getType()};
        arguments.push_back(newArg);
    }
    currentFunction = getOrCreateFunctionDefinition(node->getName(), node->getReturnType(), arguments);

    if (!node->isExternal()) {
        llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry-" + node->getName(), currentFunction);
        builder.SetInsertPoint(BB);

        withScope([this, &node]() {
            for (auto &arg : currentFunction->args()) {
                auto value = createEntryBlockAlloca(arg.getType(), arg.getName());

                // store initial value
                builder.CreateStore(&arg, value);

                definedVariables[definedVariables.size() - 1][arg.getName()] = value;
            }

            node->getBody()->accept(this);
        });
    }

    finalizeFunction(currentFunction, node->getReturnType(), node->isExternal());

    isGlobalScope = previousGlobalScopeState;
    currentFunction = previousFunction;
    // TODO(henne): this is a hack to enable us to get back to the previous functions insertion point
    //      we should save that last insertion point somewhere, instead of guessing it here
    builder.SetInsertPoint(&currentFunction->getBasicBlockList().back());

    LOG("Exit Function")
}

llvm::Function *IrGenerator::getOrCreateFunctionDefinition(const std::string &name, const ast::DataType returnType,
                                                           const std::vector<FunctionArgument> &arguments) {
    llvm::Function *function = llvmModule.getFunction(name);
    if (function == nullptr) {
        auto retType = getType(returnType);

        std::vector<llvm::Type *> argumentTypes = {};
        argumentTypes.reserve(arguments.size());
        for (auto &arg : arguments) {
            llvm::Type *type = getType(arg.type);
            if (arg.type == ast::STRING) {
                // TODO this is a hack to get strings working.
                //      ín the future we want only primitive types to be passed by value
                //      everything else is going to be passed by pointer
                type = type->getPointerTo();
            }
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
    }

    return function;
}

llvm::Function *IrGenerator::getOrCreateFunctionDefinition(const FunctionSignature &signature) {
    return getOrCreateFunctionDefinition(signature.name, signature.returnType, signature.arguments);
}

void IrGenerator::finalizeFunction(llvm::Function *function, const ast::DataType returnType,
                                   const bool isExternalFunction) {
    if (!isExternalFunction && returnType == ast::DataType::VOID) {
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
    llvm::Function *calleeFunc = llvmModule.getFunction(node->getName());
    if (calleeFunc == nullptr) {
        const FunctionResolveResult resolveResult = functionResolver.resolveFunction(module, node->getName());
        if (!resolveResult.functionExists) {
            return logError("Undefined function '" + node->getName() + "'");
        }

        calleeFunc = getOrCreateFunctionDefinition(resolveResult.signature);
        if (calleeFunc == nullptr) {
            return logError("Could not generate external definition for function '" + node->getName() + "'");
        }
    }

    if (calleeFunc->arg_size() != node->getArguments().size()) {
        return logError("Wrong number of arguments passed.");
    }

    std::vector<llvm::Value *> arguments;
    for (auto &argument : node->getArguments()) {
        argument->accept(this);
        auto itr = nodesToValues.find(argument);
        if (itr == nodesToValues.end()) {
            return logError("Could not generate code for argument.");
        }
        auto arg = itr->second;
        if (typeResolver.getTypeOf(argument) == ast::STRING) {
            // TODO this is a hack to get strings working.
            //      ín the future we want only primitive types to be passed by value
            //      everything else is going to be passed by pointer
            arg = ((llvm::LoadInst *)itr->second)->getPointerOperand();
        }
        arguments.push_back(arg);
    }

    llvm::Value *call = nullptr;
    if (calleeFunc->getReturnType()->getTypeID() == llvm::Type::TypeID::VoidTyID) {
        call = builder.CreateCall(calleeFunc, arguments);
    } else {
        call = builder.CreateCall(calleeFunc, arguments, "call");
    }
    nodesToValues[node] = call;
}
