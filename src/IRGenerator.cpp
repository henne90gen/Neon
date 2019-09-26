#include "IRGenerator.h"

#include <iostream>

void IRGenerator::logError(const std::string &msg) {
    std::cout << msg << std::endl;
}

llvm::Type *IRGenerator::getType(AstNode::DataType type) {
    switch (type) {
    case AstNode::VOID:
        return nullptr;
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
    llvm::IRBuilder<> tmpB(&currentFunction->getEntryBlock(), currentFunction->getEntryBlock().begin());
    return tmpB.CreateAlloca(type, 0, name);
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
        currentFunction = function;

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
}

void IRGenerator::visitVariableNode(VariableNode *node) {
    //    currentValue = definedVariables[node->getName()];
    //    if (currentValue == nullptr) {
    //        return logError("Undefined variable " + node->getName());
    //    }
}

void IRGenerator::visitVariableDefinitionNode(VariableDefinitionNode *node) {}

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
        nodesToValues[node] = builder.CreateFAdd(l, r, "addtmp");
        return;
    case BinaryOperationNode::MULTIPLICATION:
        nodesToValues[node] = builder.CreateFMul(l, r, "multmp");
        return;
    case BinaryOperationNode::SUBTRACTION:
        nodesToValues[node] = builder.CreateFSub(l, r, "subtmp");
        return;
    case BinaryOperationNode::DIVISION:
        nodesToValues[node] = builder.CreateFDiv(l, r, "divtmp");
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

void IRGenerator::print() { module.print(llvm::outs(), nullptr); }

void IRGenerator::visitAssignmentNode(AssignmentNode *node) { std::cerr << "Not implemented." << std::endl; }

void generateIR(AstNode *root) {
    if (root == nullptr) {
        return;
    }

    auto generator = new IRGenerator();
    root->accept(generator);

    generator->print();
}
