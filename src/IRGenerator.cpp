#include "IRGenerator.h"

#include <iostream>

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
    currentFunction = getOrCreateFunction(node->getName(), node->getReturnType(), node->getArguments());

    node->getBody()->accept(this);

    finalizeFunction(currentFunction, node->getReturnType());
    currentFunction = previousFunction;

    LOG("Exit Function")
}

void IRGenerator::visitVariableNode(VariableNode *node) {
    LOG("Enter Variable")

    if (definedVariables.find(node->getName()) == definedVariables.end()) {
        return logError("Undefined variable " + node->getName());
    }

    auto alloca = definedVariables[node->getName()];
    if (alloca == nullptr) {
        return;
    }
    nodesToValues[node] = builder.CreateLoad(alloca, node->getName());

    LOG("Exit Variable")
}

void IRGenerator::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    LOG("Enter VariableDefinition")

    auto alloca = createEntryBlockAlloca(getType(node->getType()), node->getName());
    definedVariables[node->getName()] = alloca;
    nodesToValues[node] = alloca;

    LOG("Exit VariableDefinition")
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

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry" + std::to_string((long)currentFunction), function);
    builder.SetInsertPoint(BB);

    for (auto &arg : function->args()) {
        llvm::AllocaInst *allocaInst = createEntryBlockAlloca(arg.getType(), arg.getName());

        // store initial value
        builder.CreateStore(&arg, allocaInst);

        definedVariables[arg.getName()] = allocaInst;
    }

    return function;
}

void IRGenerator::finalizeFunction(llvm::Function *function, AstNode::DataType returnType) {
    if (returnType == AstNode::VOID) {
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
                                                                 llvm::PassBuilder::ThinLTOPhase::None, true);
    //    functionPassManager = llvm::FunctionPassManager();
    functionPassManager.addPass(llvm::PromotePass());
    functionPassManager.addPass(llvm::InstCombinePass());
    //        functionPassManager.addPass(llvm::ReassociatePass()); // throws "out of memory" error
    functionPassManager.addPass(llvm::GVN());
    functionPassManager.addPass(llvm::SimplifyCFGPass());

    functionPassManager.run(*function, functionAnalysisManager);
}

void IRGenerator::visitSequenceNode(SequenceNode *node) {
    LOG("Enter Sequence")

    llvm::Function *initFunc = nullptr;
    if (currentFunction == nullptr) {
        // TODO make sure this function name does not collide with any user defined functions
        initFunc = getOrCreateFunction("tmp", AstNode::DataType::VOID, {});
        currentFunction = initFunc;
    }

    for (auto child : node->getChildren()) {
        child->accept(this);
    }

    if (initFunc != nullptr) {
        finalizeFunction(initFunc, AstNode::DataType::VOID);
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
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    llvm::Value *&src = nodesToValues[node->getRight()];
    llvm::Value *&dest = nodesToValues[node->getLeft()];
    if (src == nullptr || dest == nullptr) {
        return logError("Could not create assignment.");
    }
    nodesToValues[node] = builder.CreateStore(src, dest);
}

void IRGenerator::visitCallNode(CallNode *node) {
    // TODO implement this
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
