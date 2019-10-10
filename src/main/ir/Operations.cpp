#include "IRGenerator.h"

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
