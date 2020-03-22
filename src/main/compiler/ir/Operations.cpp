#include "IrGenerator.h"

void IrGenerator::emitIntegerOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r) {
    switch (node->getType()) {
    case ast::BinaryOperationType::ADDITION:
        nodesToValues[node] = builder.CreateAdd(l, r, "add");
        return;
    case ast::BinaryOperationType::MULTIPLICATION:
        nodesToValues[node] = builder.CreateMul(l, r, "mul");
        return;
    case ast::BinaryOperationType::SUBTRACTION:
        nodesToValues[node] = builder.CreateSub(l, r, "sub");
        return;
    case ast::BinaryOperationType::DIVISION:
        nodesToValues[node] = builder.CreateSDiv(l, r, "div");
        return;
    case ast::BinaryOperationType::EQUALS:
        nodesToValues[node] = builder.CreateICmpEQ(l, r, "eq");
        return;
    case ast::BinaryOperationType::NOT_EQUALS:
        nodesToValues[node] = builder.CreateICmpNE(l, r, "neq");
        return;
    case ast::BinaryOperationType::LESS_EQUALS:
        nodesToValues[node] = builder.CreateICmpSLE(l, r, "leq");
        return;
    case ast::BinaryOperationType::LESS_THAN:
        nodesToValues[node] = builder.CreateICmpSLT(l, r, "lt");
        return;
    case ast::BinaryOperationType::GREATER_EQUALS:
        nodesToValues[node] = builder.CreateICmpSGE(l, r, "geq");
        return;
    case ast::BinaryOperationType::GREATER_THAN:
        nodesToValues[node] = builder.CreateICmpSGT(l, r, "gt");
        return;
    }
    logError("Invalid binary operation. " + to_string(node->getType()));
}

void IrGenerator::emitFloatOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r) {
    switch (node->getType()) {
    case ast::BinaryOperationType::ADDITION:
        nodesToValues[node] = builder.CreateFAdd(l, r, "add");
        return;
    case ast::BinaryOperationType::MULTIPLICATION:
        nodesToValues[node] = builder.CreateFMul(l, r, "mul");
        return;
    case ast::BinaryOperationType::SUBTRACTION:
        nodesToValues[node] = builder.CreateFSub(l, r, "sub");
        return;
    case ast::BinaryOperationType::DIVISION:
        nodesToValues[node] = builder.CreateFDiv(l, r, "div");
        return;
    case ast::BinaryOperationType::EQUALS:
        nodesToValues[node] = builder.CreateFCmpOEQ(l, r, "eq");
        return;
    case ast::BinaryOperationType::NOT_EQUALS:
        nodesToValues[node] = builder.CreateFCmpONE(l, r, "neq");
        return;
    case ast::BinaryOperationType::LESS_EQUALS:
        nodesToValues[node] = builder.CreateFCmpOLE(l, r, "leq");
        return;
    case ast::BinaryOperationType::LESS_THAN:
        nodesToValues[node] = builder.CreateFCmpOLT(l, r, "lt");
        return;
    case ast::BinaryOperationType::GREATER_EQUALS:
        nodesToValues[node] = builder.CreateFCmpOGE(l, r, "geq");
        return;
    case ast::BinaryOperationType::GREATER_THAN:
        nodesToValues[node] = builder.CreateFCmpOGT(l, r, "gt");
        return;
    }
    logError("Invalid binary operation. " + to_string(node->getType()));
}

void IrGenerator::emitStringOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r) {
    switch (node->getType()) {
    case ast::ADDITION: {
        auto stringType = getStringType();
        std::string name = "tmpStr";
        auto result = createEntryBlockAlloca(stringType, name);

        unsigned int numCharacters = 0; // default number of characters, forces a resize to the correct size
        std::vector<llvm::Constant *> constants = {
              llvm::ConstantPointerNull::get(llvm::IntegerType::getInt8PtrTy(context)),      // content
              llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), numCharacters), // length
              llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), numCharacters), // max length
        };
        auto initializer = llvm::ConstantStruct::get(stringType, constants);
        builder.CreateStore(initializer, result);

        std::vector<llvm::Value *> args = {};
        args.push_back(result);
        args.push_back(l);
        args.push_back(r);
        createStdLibCall("appendString", args);
        nodesToValues[node] = result;

        currentScope().cleanUpFunctions.emplace_back([this, stringType, node]() {
          std::vector<llvm::Value *> args = {};
          args.push_back(nodesToValues[node]);
          createStdLibCall("deleteString", args);
        });
        return;
    }
    case ast::MULTIPLICATION:
        return;
    case ast::SUBTRACTION:
        return;
    case ast::DIVISION:
        return;
    case ast::EQUALS:
        return;
    case ast::NOT_EQUALS:
        return;
    case ast::LESS_EQUALS:
        return;
    case ast::LESS_THAN:
        return;
    case ast::GREATER_EQUALS:
        return;
    case ast::GREATER_THAN:
        return;
    }
    logError("Invalid binary operation. " + to_string(node->getType()));
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

    ast::DataType typeOfLeft = typeResolver.getTypeOf(node->getLeft());
    ast::DataType typeOfRight = typeResolver.getTypeOf(node->getRight());
    if (typeOfLeft != typeOfRight) {
        return logError("Types " + to_string(typeOfLeft) + " and " + to_string(typeOfRight) +
                        " are not compatible for binary operation");
    }

    if (typeOfLeft == ast::DataType::INT) {
        emitIntegerOperation(node, l, r);
    } else if (typeOfLeft == ast::DataType::FLOAT) {
        emitFloatOperation(node, l, r);
    } else if (typeOfLeft == ast::DataType::STRING) {
        emitStringOperation(node, l, r);
    } else {
        return logError("Binary operations are not supported for types " + to_string(typeOfLeft) + " and " +
                        to_string(typeOfRight));
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
