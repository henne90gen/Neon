#include "IrGenerator.h"

const int NUM_BITS_OF_INT = 64;

llvm::Type *IrGenerator::getType(ast::DataType type) {
    switch (type) {
    case ast::DataType::VOID:
        return llvm::Type::getVoidTy(context);
    case ast::DataType::INT:
        return llvm::Type::getInt64Ty(context);
    case ast::DataType::FLOAT:
        return llvm::Type::getDoubleTy(context);
    case ast::DataType::BOOL:
        return llvm::Type::getInt1Ty(context);
    case ast::DataType::STRING:
        return getStringType();
    default:
        return nullptr;
    }
}

llvm::StructType *IrGenerator::getStringType() {
    auto type = llvmModule.getTypeByName("string");
    if (type != nullptr) {
        return type;
    }

    std::vector<llvm::Type *> elements = {
          llvm::IntegerType::getInt8PtrTy(context), // content
          llvm::IntegerType::getInt64Ty(context),   // length
          llvm::IntegerType::getInt64Ty(context),   // max length
    };
    return llvm::StructType::create(context, elements, "string");
}

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

void IrGenerator::visitStringNode(StringNode *node) {
    const std::string stringValue = node->getValue();
    unsigned int numCharacters = stringValue.size();
    std::vector<llvm::Constant *> constants = {
          llvm::ConstantPointerNull::get(llvm::IntegerType::getInt8PtrTy(context)),      // content
          llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), numCharacters), // length
          llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), numCharacters), // max length
    };
    auto stringType = getStringType();
    auto value = llvm::ConstantStruct::get(stringType, constants);

    // two options:
    //      generate code that inits string
    //      call function that inits string
    // going with second option for now

    builder.CreateStore(value, currentDestination);

    auto data = builder.CreateGlobalStringPtr(stringValue, "str");
    std::vector<llvm::Value *> args = {};
    args.push_back(currentDestination);
    args.push_back(data);
    createStdLibCall("createString", args);

    nodesToValues[node] = currentDestination;
    currentDestination = nullptr;

    currentScope().cleanUpFunctions.emplace_back([this, stringType, node]() {
        std::vector<llvm::Value *> args = {};
        args.push_back(nodesToValues[node]);
        createStdLibCall("deleteString", args);
    });

    LOG("Created String");
}
