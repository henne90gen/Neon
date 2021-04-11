#include "IrGenerator.h"

void IrGenerator::emitIntegerOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r) {
    switch (node->type) {
    case ast::BinaryOperationType::ADDITION:
        nodesToValues[AST_NODE(node)] = builder.CreateAdd(l, r, "add");
        return;
    case ast::BinaryOperationType::MULTIPLICATION:
        nodesToValues[AST_NODE(node)] = builder.CreateMul(l, r, "mul");
        return;
    case ast::BinaryOperationType::SUBTRACTION:
        nodesToValues[AST_NODE(node)] = builder.CreateSub(l, r, "sub");
        return;
    case ast::BinaryOperationType::DIVISION:
        nodesToValues[AST_NODE(node)] = builder.CreateSDiv(l, r, "div");
        return;
    case ast::BinaryOperationType::EQUALS:
        nodesToValues[AST_NODE(node)] = builder.CreateICmpEQ(l, r, "eq");
        return;
    case ast::BinaryOperationType::NOT_EQUALS:
        nodesToValues[AST_NODE(node)] = builder.CreateICmpNE(l, r, "neq");
        return;
    case ast::BinaryOperationType::LESS_EQUALS:
        nodesToValues[AST_NODE(node)] = builder.CreateICmpSLE(l, r, "leq");
        return;
    case ast::BinaryOperationType::LESS_THAN:
        nodesToValues[AST_NODE(node)] = builder.CreateICmpSLT(l, r, "lt");
        return;
    case ast::BinaryOperationType::GREATER_EQUALS:
        nodesToValues[AST_NODE(node)] = builder.CreateICmpSGE(l, r, "geq");
        return;
    case ast::BinaryOperationType::GREATER_THAN:
        nodesToValues[AST_NODE(node)] = builder.CreateICmpSGT(l, r, "gt");
        return;
    case ast::BinaryOperationType::AND:
    case ast::BinaryOperationType::OR:
        // not defined for integers
        return;
    }
    logError("Invalid binary operation. " + to_string(node->type));
}

void IrGenerator::emitFloatOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r) {
    switch (node->type) {
    case ast::BinaryOperationType::ADDITION:
        nodesToValues[AST_NODE(node)] = builder.CreateFAdd(l, r, "add");
        return;
    case ast::BinaryOperationType::MULTIPLICATION:
        nodesToValues[AST_NODE(node)] = builder.CreateFMul(l, r, "mul");
        return;
    case ast::BinaryOperationType::SUBTRACTION:
        nodesToValues[AST_NODE(node)] = builder.CreateFSub(l, r, "sub");
        return;
    case ast::BinaryOperationType::DIVISION:
        nodesToValues[AST_NODE(node)] = builder.CreateFDiv(l, r, "div");
        return;
    case ast::BinaryOperationType::EQUALS:
        nodesToValues[AST_NODE(node)] = builder.CreateFCmpOEQ(l, r, "eq");
        return;
    case ast::BinaryOperationType::NOT_EQUALS:
        nodesToValues[AST_NODE(node)] = builder.CreateFCmpONE(l, r, "neq");
        return;
    case ast::BinaryOperationType::LESS_EQUALS:
        nodesToValues[AST_NODE(node)] = builder.CreateFCmpOLE(l, r, "leq");
        return;
    case ast::BinaryOperationType::LESS_THAN:
        nodesToValues[AST_NODE(node)] = builder.CreateFCmpOLT(l, r, "lt");
        return;
    case ast::BinaryOperationType::GREATER_EQUALS:
        nodesToValues[AST_NODE(node)] = builder.CreateFCmpOGE(l, r, "geq");
        return;
    case ast::BinaryOperationType::GREATER_THAN:
        nodesToValues[AST_NODE(node)] = builder.CreateFCmpOGT(l, r, "gt");
        return;
    case ast::BinaryOperationType::AND:
    case ast::BinaryOperationType::OR:
        // not defined for floats
        return;
    }
    logError("Invalid binary operation. " + to_string(node->type));
}

void IrGenerator::emitStringOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r) {
    switch (node->type) {
    case ast::BinaryOperationType::ADDITION: {
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
        nodesToValues[AST_NODE(node)] = result;

        currentScope().cleanUpFunctions.emplace_back([this, stringType, node]() {
            auto *loadedValue = builder.CreateLoad(nodesToValues[AST_NODE(node)]);
            std::vector<llvm::Value *> args = {};
            args.push_back(loadedValue);
            createStdLibCall("deleteString", args);
        });
        return;
    }
    case ast::BinaryOperationType::MULTIPLICATION:
    case ast::BinaryOperationType::SUBTRACTION:
    case ast::BinaryOperationType::DIVISION:
    case ast::BinaryOperationType::EQUALS:
    case ast::BinaryOperationType::NOT_EQUALS:
    case ast::BinaryOperationType::LESS_EQUALS:
    case ast::BinaryOperationType::LESS_THAN:
    case ast::BinaryOperationType::GREATER_EQUALS:
    case ast::BinaryOperationType::GREATER_THAN:
    case ast::BinaryOperationType::AND:
    case ast::BinaryOperationType::OR:
        // not defined for strings
        return;
    }
    logError("Invalid binary operation: " + to_string(node->type));
}

void IrGenerator::emitBooleanOperation(BinaryOperationNode *node, llvm::Value *l, llvm::Value *r) {
    switch (node->type) {
    case ast::BinaryOperationType::AND:
        nodesToValues[AST_NODE(node)] = builder.CreateAnd(l, r, "and");
        return;
    case ast::BinaryOperationType::OR:
        nodesToValues[AST_NODE(node)] = builder.CreateOr(l, r, "or");
        return;
    case ast::BinaryOperationType::ADDITION:
    case ast::BinaryOperationType::MULTIPLICATION:
    case ast::BinaryOperationType::SUBTRACTION:
    case ast::BinaryOperationType::DIVISION:
    case ast::BinaryOperationType::EQUALS:
    case ast::BinaryOperationType::NOT_EQUALS:
    case ast::BinaryOperationType::LESS_EQUALS:
    case ast::BinaryOperationType::LESS_THAN:
    case ast::BinaryOperationType::GREATER_EQUALS:
    case ast::BinaryOperationType::GREATER_THAN:
        // not defined for booleans
        return;
    }
    logError("Invalid binary operation: " + to_string(node->type));
}

void IrGenerator::visitBinaryOperationNode(BinaryOperationNode *node) {
    log.debug("Enter BinaryOperation");

    visitNode(node->left);
    auto *l = nodesToValues[node->left];
    visitNode(node->right);
    auto *r = nodesToValues[node->right];

    if (l == nullptr || r == nullptr) {
        return logError("Generating left or right side failed.");
    }

    ast::DataType typeOfLeft = typeResolver.getTypeOf(module, node->left);
    ast::DataType typeOfRight = typeResolver.getTypeOf(module, node->right);
    if (typeOfLeft != typeOfRight) {
        return logError("Types " + to_string(typeOfLeft) + " and " + to_string(typeOfRight) +
                        " are not compatible for binary operation");
    }

    if (typeOfLeft == ast::DataType(ast::SimpleDataType::INTEGER)) {
        emitIntegerOperation(node, l, r);
    } else if (typeOfLeft == ast::DataType(ast::SimpleDataType::FLOAT)) {
        emitFloatOperation(node, l, r);
    } else if (typeOfLeft == ast::DataType(ast::SimpleDataType::STRING)) {
        emitStringOperation(node, l, r);
    } else if (typeOfLeft == ast::DataType(ast::SimpleDataType::BOOLEAN)) {
        emitBooleanOperation(node, l, r);
    } else {
        return logError("Binary operations are not supported for types " + to_string(typeOfLeft) + " and " +
                        to_string(typeOfRight));
    }

    log.debug("Exit BinaryOperation");
}

void IrGenerator::visitUnaryOperationNode(UnaryOperationNode *node) {
    log.debug("Enter UnaryOperation");

    visitNode(node->child);
    auto *c = nodesToValues[AST_NODE(node->child)];
    if (c == nullptr) {
        return logError("Generating the child failed.");
    }

    switch (node->type) {
    case ast::UnaryOperationType::NOT:
        nodesToValues[AST_NODE(node)] = builder.CreateNot(c, "not");
        break;
    case ast::UnaryOperationType::NEGATE: {
        const ast::DataType &type = typeResolver.getTypeOf(module, AST_NODE(node));
        if (type == ast::DataType(ast::INTEGER)) {
            nodesToValues[AST_NODE(node)] = builder.CreateNeg(c, "neg");
        } else if (type == ast::DataType(ast::FLOAT)) {
            nodesToValues[AST_NODE(node)] = builder.CreateFNeg(c, "neg");
        } else {
            logError("Type does not support negation: " + to_string(type));
        }
    } break;
    default:
        logError("Invalid unary operation: " + to_string(node->type));
        break;
    }

    log.debug("Exit UnaryOperation");
}
