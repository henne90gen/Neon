#include "IrGenerator.h"

void IrGenerator::emitIntegerOperation(BinaryOperationNode *node, llvm::Value*l, llvm::Value*r) {
    switch (node->getType()) {
    case ast::BinaryOperationType::ADDITION:
        nodesToValues[node] = builder.CreateAdd(l, r, "add");
        break;
    case ast::BinaryOperationType::MULTIPLICATION:
        nodesToValues[node] = builder.CreateMul(l, r, "mul");
        break;
    case ast::BinaryOperationType::SUBTRACTION:
        nodesToValues[node] = builder.CreateSub(l, r, "sub");
        break;
    case ast::BinaryOperationType::DIVISION:
        nodesToValues[node] = builder.CreateSDiv(l, r, "div");
        break;
    case ast::BinaryOperationType::EQUALS:
        nodesToValues[node] = builder.CreateICmpEQ(l, r, "eq");
        break;
    case ast::BinaryOperationType::NOT_EQUALS:
        nodesToValues[node] = builder.CreateICmpNE(l, r, "neq");
        break;
    case ast::BinaryOperationType::LESS_EQUALS:
        nodesToValues[node] = builder.CreateICmpSLE(l, r, "leq");
        break;
    case ast::BinaryOperationType::LESS_THAN:
        nodesToValues[node] = builder.CreateICmpSLT(l, r, "lt");
        break;
    case ast::BinaryOperationType::GREATER_EQUALS:
        nodesToValues[node] = builder.CreateICmpSGE(l, r, "geq");
        break;
    case ast::BinaryOperationType::GREATER_THAN:
        nodesToValues[node] = builder.CreateICmpSGT(l, r, "gt");
        break;
    default:
        logError("Invalid binary operation. " + std::to_string(node->getType()));
        break;
    }
}

void IrGenerator::emitFloatDivision(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r) {
    switch (node->getType()) {
    case ast::BinaryOperationType::ADDITION:
        nodesToValues[node] = builder.CreateFAdd(l, r, "add");
        break;
    case ast::BinaryOperationType::MULTIPLICATION:
        nodesToValues[node] = builder.CreateFMul(l, r, "mul");
        break;
    case ast::BinaryOperationType::SUBTRACTION:
        nodesToValues[node] = builder.CreateFSub(l, r, "sub");
        break;
    case ast::BinaryOperationType::DIVISION:
        nodesToValues[node] = builder.CreateFDiv(l, r, "div");
        break;
    case ast::BinaryOperationType::EQUALS:
        nodesToValues[node] = builder.CreateFCmpOEQ(l, r, "eq");
        break;
    case ast::BinaryOperationType::NOT_EQUALS:
        nodesToValues[node] = builder.CreateFCmpONE(l, r, "neq");
        break;
    case ast::BinaryOperationType::LESS_EQUALS:
        nodesToValues[node] = builder.CreateFCmpOLE(l, r, "leq");
        break;
    case ast::BinaryOperationType::LESS_THAN:
        nodesToValues[node] = builder.CreateFCmpOLT(l, r, "lt");
        break;
    case ast::BinaryOperationType::GREATER_EQUALS:
        nodesToValues[node] = builder.CreateFCmpOGE(l, r, "geq");
        break;
    case ast::BinaryOperationType::GREATER_THAN:
        nodesToValues[node] = builder.CreateFCmpOGT(l, r, "gt");
        break;
    default:
        logError("Invalid binary operation. " + std::to_string(node->getType()));
        break;
    }
}

void IrGenerator::visitBinaryOperationNode(BinaryOperationNode *node) {
    LOG("Enter BinaryOperation")

    node->getLeft()->accept(this);
    auto l = nodesToValues[node->getLeft()];
    node->getRight()->accept(this);
    auto r = nodesToValues[node->getRight()];

    if (l == nullptr || r == nullptr) {
        return logError("Generating left or right side failed.");
    }

    if (typeResolver.getTypeOf(node->getLeft()) == ast::DataType::INT) {
        emitIntegerOperation(node, l, r);
    } else if (typeResolver.getTypeOf(node->getLeft()) == ast::DataType::FLOAT) {
        emitFloatDivision(node, l, r);
    }

    LOG("Exit BinaryOperation")
}

void IrGenerator::visitUnaryOperationNode(UnaryOperationNode *node) {
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
