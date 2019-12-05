#include "IrGenerator.h"

const int NUM_BITS_OF_INT = 64;

void IrGenerator::visitIntegerNode(IntegerNode *node) {
    nodesToValues[node] = llvm::ConstantInt::get(context, llvm::APInt(NUM_BITS_OF_INT, node->getValue()));
    LOG("Created Integer")
}

void IrGenerator::visitFloatNode(FloatNode *node) {
    nodesToValues[node] = llvm::ConstantFP::get(context, llvm::APFloat(node->getValue()));
    LOG("Created Float")
}

void IrGenerator::visitBoolNode(BoolNode *node) {
    nodesToValues[node] = llvm::ConstantInt::get(context, llvm::APInt(1, static_cast<uint64_t>(node->getValue())));
    LOG("Created Bool")
}
