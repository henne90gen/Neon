#include "IRGenerator.h"

void IRGenerator::visitIntegerNode(IntegerNode *node) {
    nodesToValues[node] = llvm::ConstantInt::get(context, llvm::APInt(32, node->getValue()));
    LOG("Created Integer")
}

void IRGenerator::visitFloatNode(FloatNode *node) {
    nodesToValues[node] = llvm::ConstantFP::get(context, llvm::APFloat(node->getValue()));
    LOG("Created Float")
}

void IRGenerator::visitBoolNode(BoolNode *node) {
    nodesToValues[node] = llvm::ConstantInt::get(context, llvm::APInt(1, static_cast<uint64_t>(node->getValue())));
    LOG("Created Bool")
}

void IRGenerator::visitAssignmentNode(AssignmentNode *node) {
    llvm::Value *dest = nullptr;
    if (node->getLeft()->getAstNodeType() == AstNode::AstNodeType::VARIABLE_DEFINITION) {
        // only generate variable definitions
        node->getLeft()->accept(this);
        dest = nodesToValues[node->getLeft()];
    } else {
        auto variable = dynamic_cast<VariableNode *>(node->getLeft());
        dest = definedVariables[variable->getName()];
    }

    node->getRight()->accept(this);
    llvm::Value *src = nodesToValues[node->getRight()];
    if (src == nullptr || dest == nullptr) {
        return logError("Could not create assignment.");
    }
    nodesToValues[node] = builder.CreateStore(src, dest);
}
