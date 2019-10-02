#include "IRGenerator.h"
#include "ObjectFileWriter.h"

#include <iostream>

void logError(const std::string &msg) { std::cerr << msg << std::endl; }

IRGenerator::IRGenerator(const Program &program)
    : program(program), builder(context), module(program.fileName, context) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    llvm::PassBuilder PB;
    PB.registerFunctionAnalyses(functionAnalysisManager);
    functionPassManager = llvm::FunctionPassManager();
    functionPassManager.addPass(llvm::PromotePass());
    functionPassManager.addPass(llvm::InstCombinePass());
    //        functionPassManager.addPass(llvm::ReassociatePass()); // throws out of memory error
    functionPassManager.addPass(llvm::GVN());
    functionPassManager.addPass(llvm::SimplifyCFGPass());
}

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
    llvm::Function *function = module.getFunction(node->getName());
    if (function == nullptr) {
        auto retType = getType(node->getReturnType());

        std::vector<llvm::Type *> argumentTypes = {};
        for (auto &arg : node->getArguments()) {
            argumentTypes.push_back(getType(arg->getType()));
        }

        llvm::FunctionType *functionType = llvm::FunctionType::get(retType, argumentTypes, false);

        function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, node->getName(), module);
        if (function == nullptr) {
            return logError("Could not create function");
        }

        unsigned int i = 0;
        for (auto &arg : function->args()) {
            arg.setName(node->getArguments()[i++]->getName());
        }
    }

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry", function);
    builder.SetInsertPoint(BB);

    for (auto &arg : function->args()) {
        llvm::AllocaInst *allocaInst = createEntryBlockAlloca(arg.getType(), arg.getName());

        // store initial value
        builder.CreateStore(&arg, allocaInst);

        definedVariables[arg.getName()] = allocaInst;
    }

    node->getBody()->accept(this);

    if (node->getReturnType() == AstNode::VOID) {
        builder.CreateRetVoid();
    }

    llvm::verifyFunction(*function);

    functionPassManager.run(*function, functionAnalysisManager);
}

void IRGenerator::visitVariableNode(VariableNode *node) {
    if (definedVariables.find(node->getName()) == definedVariables.end()) {
        return logError("Undefined variable " + node->getName());
    }
    auto alloca = definedVariables[node->getName()];
    if (alloca == nullptr) {
        return;
    }
    nodesToValues[node] = builder.CreateLoad(alloca, node->getName());
}

void IRGenerator::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    auto alloca = createEntryBlockAlloca(getType(node->getType()), node->getName());
    definedVariables[node->getName()] = alloca;
    nodesToValues[node] = alloca;
}

void IRGenerator::visitBinaryOperationNode(BinaryOperationNode *node) {
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
        return;
    case BinaryOperationNode::MULTIPLICATION:
        nodesToValues[node] = builder.CreateMul(l, r, "mul");
        return;
    case BinaryOperationNode::SUBTRACTION:
        nodesToValues[node] = builder.CreateSub(l, r, "sub");
        return;
    case BinaryOperationNode::DIVISION:
        nodesToValues[node] = builder.CreateFDiv(l, r, "div");
        return;
    default:
        return logError("Invalid binary operation.");
    }
}

void IRGenerator::visitUnaryOperationNode(UnaryOperationNode *node) {
    node->getChild()->accept(this);
    auto c = nodesToValues[node->getChild()];
    if (c == nullptr) {
        return logError("Generating the child failed.");
    }

    switch (node->getType()) {
    case UnaryOperationNode::NOT:
        nodesToValues[node] = builder.CreateNot(c, "nottmp");
        return;
    default:
        return logError("Invalid unary operation.");
    }
}

void IRGenerator::visitSequenceNode(SequenceNode *node) {
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
}

void IRGenerator::visitStatementNode(StatementNode *node) {
    if (node->getChild() == nullptr) {
        return;
    }

    node->getChild()->accept(this);
    if (node->isReturnStatement()) {
        builder.CreateRet(nodesToValues[node->getChild()]);
    }
}

void IRGenerator::visitIntegerNode(IntegerNode *node) {
    nodesToValues[node] = llvm::ConstantInt::get(context, llvm::APInt(32, node->getValue()));
}

void IRGenerator::visitFloatNode(FloatNode *node) {
    nodesToValues[node] = llvm::ConstantFP::get(context, llvm::APFloat(node->getValue()));
}

void IRGenerator::visitBoolNode(BoolNode *node) {
    nodesToValues[node] = llvm::ConstantInt::get(context, llvm::APInt(1, node->getValue()));
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

void IRGenerator::print() {
    std::string fileName = program.fileName + ".llvm";
    std::error_code EC;
    llvm::raw_fd_ostream dest(fileName, EC, llvm::sys::fs::OF_None);
    module.print(llvm::outs(), nullptr);
    module.print(dest, nullptr);
}

void IRGenerator::run(AstNode *root) {
    if (root == nullptr) {
        return;
    }

    root->accept(this);

    this->print();
}
