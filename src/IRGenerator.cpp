#include "IRGenerator.h"

#include <iostream>
#include <llvm/Support/FileSystem.h>

#define LOG(msg)                                                                                                       \
    if (verbose) {                                                                                                     \
        std::cout << msg << std::endl;                                                                                 \
    }

void logError(const std::string &msg) { std::cerr << msg << std::endl; }

IRGenerator::IRGenerator(const Program &program, const bool verbose = false)
    : program(program), verbose(verbose), builder(context), module(program.fileName, context) {}

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

void IRGenerator::visitFunctionNode(FunctionNode *node) {
    LOG("Enter Function")

    llvm::Function *previousFunction = currentFunction;
    bool previousGlobalScopeState = isGlobalScope;
    isGlobalScope = false;
    currentFunction = getOrCreateFunction(node->getName(), node->getReturnType(), node->getArguments());

    node->getBody()->accept(this);

    finalizeFunction(currentFunction, node->getReturnType());

    isGlobalScope = previousGlobalScopeState;
    currentFunction = previousFunction;
    // TODO this is a hack to enable us to get back to the previous functions insertion point
    //      we should save that last insertion point somewhere, instead of guessing it here
    builder.SetInsertPoint(&currentFunction->getBasicBlockList().back());

    LOG("Exit Function")
}

void IRGenerator::visitVariableNode(VariableNode *node) {
    LOG("Enter Variable")

    if (definedVariables.find(node->getName()) == definedVariables.end()) {
        return logError("Undefined variable " + node->getName());
    }

    auto value = definedVariables[node->getName()];
    if (value == nullptr) {
        return;
    }
    nodesToValues[node] = builder.CreateLoad(value, node->getName());

    LOG("Exit Variable")
}

void IRGenerator::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    LOG("Enter VariableDefinition")

    llvm::Type *type = getType(node->getType());
    std::string &name = node->getName();
    llvm::Value *value = nullptr;
    if (isGlobalScope) {
        value = module.getOrInsertGlobal(name, type);
        module.getNamedGlobal(name)->setDSOLocal(true);
        module.getNamedGlobal(name)->setInitializer(getInitializer(node->getType()));
    } else {
        value = createEntryBlockAlloca(type, name);
    }
    definedVariables[name] = value;
    nodesToValues[node] = value;

    LOG("Exit VariableDefinition")
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

void IRGenerator::visitBinaryOperationNode(BinaryOperationNode *node) {
    LOG("Enter BinaryOperation")

    node->getLeft()->accept(this);
    auto l = nodesToValues[node->getLeft()];
    node->getRight()->accept(this);
    auto r = nodesToValues[node->getRight()];

    if (l == nullptr || r == nullptr) {
        return logError("Generating left or right side failed.");
    }

    switch (node->getType()) {
    case BinaryOperationNode::ADDITION:
        nodesToValues[node] = builder.CreateAdd(l, r, "add");
        break;
    case BinaryOperationNode::MULTIPLICATION:
        nodesToValues[node] = builder.CreateMul(l, r, "mul");
        break;
    case BinaryOperationNode::SUBTRACTION:
        nodesToValues[node] = builder.CreateSub(l, r, "sub");
        break;
    case BinaryOperationNode::DIVISION:
        nodesToValues[node] = builder.CreateFDiv(l, r, "div");
        break;
    default:
        logError("Invalid binary operation.");
        break;
    }

    LOG("Exit BinaryOperation")
}

void IRGenerator::visitUnaryOperationNode(UnaryOperationNode *node) {
    LOG("Enter UnaryOperation")

    node->getChild()->accept(this);
    auto c = nodesToValues[node->getChild()];
    if (c == nullptr) {
        return logError("Generating the child failed.");
    }

    switch (node->getType()) {
    case UnaryOperationNode::NOT:
        nodesToValues[node] = builder.CreateNot(c, "not");
        break;
    default:
        logError("Invalid unary operation.");
        break;
    }

    LOG("Exit UnaryOperation")
}

llvm::Function *IRGenerator::getOrCreateFunction(const std::string &name, AstNode::DataType returnType,
                                                 const std::vector<VariableDefinitionNode *> &arguments) {
    llvm::Function *function = module.getFunction(name);
    if (function == nullptr) {
        auto retType = getType(returnType);

        std::vector<llvm::Type *> argumentTypes = {};
        for (auto &arg : arguments) {
            argumentTypes.push_back(getType(arg->getType()));
        }

        llvm::FunctionType *functionType = llvm::FunctionType::get(retType, argumentTypes, false);

        function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, name, module);
        if (function == nullptr) {
            logError("Could not create function");
            return nullptr;
        }

        unsigned int i = 0;
        for (auto &arg : function->args()) {
            arg.setName(arguments[i++]->getName());
        }
    }

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry-" + name, function);
    builder.SetInsertPoint(BB);

    for (auto &arg : function->args()) {
        auto value = createEntryBlockAlloca(arg.getType(), arg.getName());

        // store initial value
        builder.CreateStore(&arg, value);

        definedVariables[arg.getName()] = value;
    }

    return function;
}

void IRGenerator::finalizeFunction(llvm::Function *function, AstNode::DataType returnType) {
    if (returnType == AstNode::VOID) {
        builder.SetInsertPoint(&function->getBasicBlockList().back());
        builder.CreateRetVoid();
    }

    if (llvm::verifyFunction(*function, &llvm::errs())) {
        print(false);
        return;
    }

    llvm::FunctionAnalysisManager functionAnalysisManager = llvm::FunctionAnalysisManager();
    llvm::FunctionPassManager functionPassManager;
    llvm::PassBuilder PB;
    PB.registerFunctionAnalyses(functionAnalysisManager);
    functionPassManager = PB.buildFunctionSimplificationPipeline(llvm::PassBuilder::OptimizationLevel::O0,
                                                                 llvm::PassBuilder::ThinLTOPhase::None);
    // functionPassManager = llvm::FunctionPassManager();
    //    functionPassManager.addPass(llvm::PromotePass());
    //    functionPassManager.addPass(llvm::InstCombinePass());
    // functionPassManager.addPass(llvm::ReassociatePass()); // throws "out of memory" error
    //    functionPassManager.addPass(llvm::GVN());
    //    functionPassManager.addPass(llvm::SimplifyCFGPass());

    functionPassManager.run(*function, functionAnalysisManager);
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
        initFunc = getOrCreateFunction("__ctor", AstNode::DataType::VOID, {});
        currentFunction = initFunc;
        isGlobalScope = true;
    }

    for (auto child : node->getChildren()) {
        child->accept(this);
    }

    if (initFunc != nullptr) {
        isGlobalScope = false;
        finalizeFunction(initFunc, AstNode::DataType::VOID);
        // TODO don't generate global init function, if there are no globals
        setupGlobalInitialization(initFunc);
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

void IRGenerator::visitIntegerNode(IntegerNode *node) {
    nodesToValues[node] = llvm::ConstantInt::get(context, llvm::APInt(32, node->getValue()));
    LOG("Created Integer")
}

void IRGenerator::visitFloatNode(FloatNode *node) {
    nodesToValues[node] = llvm::ConstantFP::get(context, llvm::APFloat(node->getValue()));
    LOG("Created Float")
}

void IRGenerator::visitBoolNode(BoolNode *node) {
    nodesToValues[node] = llvm::ConstantInt::get(context, llvm::APInt(1, node->getValue()));
    LOG("Created Bool")
}

void IRGenerator::visitAssignmentNode(AssignmentNode *node) {
    llvm::Value *dest = nullptr;
    if (node->getLeft()->getAstNodeType() == AstNode::AstNodeType::VARIABLE_DEFINITION) {
        // only generate variable definitions
        node->getLeft()->accept(this);
        dest = nodesToValues[node->getLeft()];
    } else {
        auto variable = (VariableNode *)node->getLeft();
        dest = definedVariables[variable->getName()];
    }

    node->getRight()->accept(this);
    llvm::Value *src = nodesToValues[node->getRight()];
    if (src == nullptr || dest == nullptr) {
        return logError("Could not create assignment.");
    }
    nodesToValues[node] = builder.CreateStore(src, dest);
}

void IRGenerator::visitCallNode(CallNode *node) {
    llvm::Function *calleeFunc = module.getFunction(node->getName());
    if (!calleeFunc)
        return logError("Use of undeclared identifier " + node->getName());

    if (calleeFunc->arg_size() != node->getArguments().size())
        return logError("Incorrect number arguments passed.");

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
