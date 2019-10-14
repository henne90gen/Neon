#include "IRGenerator.h"

#include <llvm/IR/Verifier.h>
#include <llvm/Passes/PassBuilder.h>

void IRGenerator::visitFunctionNode(FunctionNode *node) {
    LOG("Enter Function")

    llvm::Function *previousFunction = currentFunction;
    bool previousGlobalScopeState = isGlobalScope;
    isGlobalScope = false;
    currentFunction = getOrCreateFunctionDefinition(node->getName(), node->getReturnType(), node->getArguments());

    if (!node->isExternal()) {
        llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry-" + node->getName(), currentFunction);
        builder.SetInsertPoint(BB);

        for (auto &arg : currentFunction->args()) {
            auto value = createEntryBlockAlloca(arg.getType(), arg.getName());

            // store initial value
            builder.CreateStore(&arg, value);

            definedVariables[arg.getName()] = value;
        }

        node->getBody()->accept(this);
    }

    finalizeFunction(currentFunction, node->getReturnType(), node->isExternal());

    isGlobalScope = previousGlobalScopeState;
    currentFunction = previousFunction;
    // TODO(henne): this is a hack to enable us to get back to the previous functions insertion point
    //      we should save that last insertion point somewhere, instead of guessing it here
    builder.SetInsertPoint(&currentFunction->getBasicBlockList().back());

    LOG("Exit Function")
}

llvm::Function *IRGenerator::getOrCreateFunctionDefinition(const std::string &name, const ast::DataType returnType,
                                                           const std::vector<VariableDefinitionNode *> &arguments) {
    llvm::Function *function = module.getFunction(name);
    if (function == nullptr) {
        auto retType = getType(returnType);

        std::vector<llvm::Type *> argumentTypes = {};
        argumentTypes.reserve(arguments.size());
        for (auto &arg : arguments) {
            argumentTypes.push_back(getType(arg->getType()));
        }

        llvm::FunctionType *functionType = llvm::FunctionType::get(retType, argumentTypes, false);

        function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, name, module);
        if (function == nullptr) {
            logError("Could not create function");
            return nullptr;
        }

        // TODO henne: clang applies these attributes somehow. But doing it here does not work
        //  llvm::AttributeList attrs = {};
        //  attrs = attrs.addAttribute(context, 0, llvm::Attribute::AttrKind::NoInline);
        //  attrs = attrs.addAttribute(context, 0, llvm::Attribute::AttrKind::OptimizeNone);
        //  attrs = attrs.addAttribute(context, 0, llvm::Attribute::AttrKind::UWTable);
        //  function->setAttributes(attrs);

        unsigned int i = 0;
        for (auto &arg : function->args()) {
            arg.setName(arguments[i++]->getName());
        }
    }

    return function;
}

void IRGenerator::finalizeFunction(llvm::Function *function, const ast::DataType returnType,
                                   const bool isExternalFunction) {
    if (!isExternalFunction && returnType == ast::DataType::VOID) {
        builder.SetInsertPoint(&function->getBasicBlockList().back());
        builder.CreateRetVoid();
    }

    if (llvm::verifyFunction(*function, &llvm::errs())) {
        print(false);
        return;
    }

    //    function->viewCFG();
    print(false);

    if (!isExternalFunction) {
        llvm::FunctionAnalysisManager functionAnalysisManager = llvm::FunctionAnalysisManager();
        llvm::FunctionPassManager functionPassManager;
        llvm::PassBuilder PB;
        PB.registerFunctionAnalyses(functionAnalysisManager);
        functionPassManager = PB.buildFunctionSimplificationPipeline(llvm::PassBuilder::OptimizationLevel::O0,
                                                                     llvm::PassBuilder::ThinLTOPhase::None, true);
        functionPassManager.run(*function, functionAnalysisManager);
    }
}

void IRGenerator::visitCallNode(CallNode *node) {
    llvm::Function *calleeFunc = module.getFunction(node->getName());
    if (calleeFunc == nullptr) {
        return logError("Use of undeclared identifier " + node->getName());
    }

    if (calleeFunc->arg_size() != node->getArguments().size()) {
        return logError("Incorrect number arguments passed.");
    }

    std::vector<llvm::Value *> arguments;
    for (auto &argument : node->getArguments()) {
        argument->accept(this);
        auto itr = nodesToValues.find(argument);
        if (itr == nodesToValues.end()) {
            return logError("Could not generate code for argument.");
        }
        arguments.push_back(itr->second);
    }

    llvm::Value *call = nullptr;
    if (calleeFunc->getReturnType()->getTypeID() == llvm::Type::TypeID::VoidTyID) {
        call = builder.CreateCall(calleeFunc, arguments);
    } else {
        call = builder.CreateCall(calleeFunc, arguments, "call");
    }
    nodesToValues[node] = call;
}
