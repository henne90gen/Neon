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
    case ast::AND:
    case ast::OR:
        // not defined for integers
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
    case ast::AND:
    case ast::OR:
        // not defined for floats
        return;
    }
    logError("Invalid binary operation. " + to_string(node->getType()));
}

void IrGenerator::emitStringOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r) {
    switch (node->getType()) {
    case ast::ADDITION: {
        auto *stringType = getStringType();
        std::string name = "tmpStr";
        auto *result = createEntryBlockAlloca(stringType->getPointerTo(), name);

        unsigned int numCharacters = 0; // default number of characters, forces a resize to the correct size
        auto *data = builder.CreateGlobalStringPtr("", "str");
        auto *size = llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), numCharacters);
        auto *maxSize = llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), numCharacters);

        std::vector<llvm::Value *> args = {};
        args.push_back(data);
        args.push_back(size);
        args.push_back(maxSize);
        auto *value = createStdLibCall("createString", args);

        builder.CreateStore(value, result);

        auto *dest = builder.CreateLoad(result);
        auto *loadedLeft = builder.CreateLoad(l);
        auto *loadedRight = builder.CreateLoad(r);
        args.clear();
        args.push_back(dest);
        args.push_back(loadedLeft);
        args.push_back(loadedRight);
        createStdLibCall("appendString", args);
        nodesToValues[node] = result;

        currentScope().cleanUpFunctions.emplace_back([this, stringType, node]() {
            auto *loadedValue = builder.CreateLoad(nodesToValues[node]);
            std::vector<llvm::Value *> args = {};
            args.push_back(loadedValue);
            createStdLibCall("deleteString", args);
        });
        return;
    }
    case ast::MULTIPLICATION:
    case ast::SUBTRACTION:
    case ast::DIVISION:
    case ast::EQUALS:
    case ast::NOT_EQUALS:
    case ast::LESS_EQUALS:
    case ast::LESS_THAN:
    case ast::GREATER_EQUALS:
    case ast::GREATER_THAN:
    case ast::AND:
    case ast::OR:
        // not defined for strings
        return;
    }
    logError("Invalid binary operation: " + to_string(node->getType()));
}

void IrGenerator::emitBooleanOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r) {
    switch (node->getType()) {
    case ast::AND:
        nodesToValues[node] = builder.CreateAnd(l, r, "and");
        return;
    case ast::OR:
        nodesToValues[node] = builder.CreateOr(l, r, "or");
        return;
    case ast::ADDITION:
    case ast::MULTIPLICATION:
    case ast::SUBTRACTION:
    case ast::DIVISION:
    case ast::EQUALS:
    case ast::NOT_EQUALS:
    case ast::LESS_EQUALS:
    case ast::LESS_THAN:
    case ast::GREATER_EQUALS:
    case ast::GREATER_THAN:
        // not defined for booleans
        return;
    }
    logError("Invalid binary operation: " + to_string(node->getType()));
}

void IrGenerator::visitBinaryOperationNode(BinaryOperationNode *node) {
    log.debug("Enter BinaryOperation");

    node->getLeft()->accept(this);
    auto *l = nodesToValues[node->getLeft()];
    node->getRight()->accept(this);
    auto *r = nodesToValues[node->getRight()];

    if (l == nullptr || r == nullptr) {
        return logError("Generating left or right side failed.");
    }

    ast::DataType typeOfLeft = typeResolver.getTypeOf(module, node->getLeft());
    ast::DataType typeOfRight = typeResolver.getTypeOf(module, node->getRight());
    if (typeOfLeft != typeOfRight) {
        return logError("Types " + to_string(typeOfLeft) + " and " + to_string(typeOfRight) +
                        " are not compatible for binary operation");
    }

    if (typeOfLeft == ast::DataType(ast::SimpleDataType::INT)) {
        emitIntegerOperation(node, l, r);
    } else if (typeOfLeft == ast::DataType(ast::SimpleDataType::FLOAT)) {
        emitFloatOperation(node, l, r);
    } else if (typeOfLeft == ast::DataType(ast::SimpleDataType::STRING)) {
        emitStringOperation(node, l, r);
    } else if (typeOfLeft == ast::DataType(ast::SimpleDataType::BOOL)) {
        emitBooleanOperation(node, l, r);
    } else {
        return logError("Binary operations are not supported for types " + to_string(typeOfLeft) + " and " +
                        to_string(typeOfRight));
    }

    log.debug("Exit BinaryOperation");
}

void IrGenerator::visitUnaryOperationNode(UnaryOperationNode *node) {
    log.debug("Enter UnaryOperation");

    node->getChild()->accept(this);
    auto *c = nodesToValues[node->getChild()];
    if (c == nullptr) {
        return logError("Generating the child failed.");
    }

    switch (node->getType()) {
    case ast::UnaryOperationType::NOT:
        nodesToValues[node] = builder.CreateNot(c, "not");
        break;
    case ast::UnaryOperationType::NEGATE: {
        const ast::DataType &type = typeResolver.getTypeOf(module, node);
        if (type == ast::DataType(ast::INT)) {
            nodesToValues[node] = builder.CreateNeg(c, "neg");
        } else if (type == ast::DataType(ast::FLOAT)) {
            nodesToValues[node] = builder.CreateFNeg(c, "neg");
        } else {
            logError("Type does not support negation: " + to_string(type));
        }
    } break;
    default:
        logError("Invalid unary operation: " + to_string(node->getType()));
        break;
    }

    log.debug("Exit UnaryOperation");
}
