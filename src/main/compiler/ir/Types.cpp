#include "IrGenerator.h"

const int NUM_BITS_OF_INT = 64;

llvm::Type *IrGenerator::getType(const ast::DataType &type) {
    bool isSimpleType = ast::isSimpleDataType(type);
    if (isSimpleType) {
        ast::SimpleDataType simpleDataType = toSimpleDataType(type);
        switch (simpleDataType) {
        case ast::SimpleDataType::VOID:
            return llvm::Type::getVoidTy(context);
        case ast::SimpleDataType::INT:
            return llvm::Type::getInt64Ty(context);
        case ast::SimpleDataType::FLOAT:
            return llvm::Type::getDoubleTy(context);
        case ast::SimpleDataType::BOOL:
            return llvm::Type::getInt1Ty(context);
        case ast::SimpleDataType::STRING:
            return getStringType()->getPointerTo();
        default:
            return nullptr;
        }
    } else {
        TypeResolveResult resolveResult = typeResolver.resolveType(module, type);
        if (!resolveResult.typeExists) {
            logError("Undefined type '" + to_string(type) + "'");
            return llvm::Type::getVoidTy(context);
        }

        auto *result = getOrCreateComplexType(resolveResult.complexType);
        if (result == nullptr) {
            logError("Could not generate type declaration for type '" + to_string(type) + "'");
            return llvm::Type::getVoidTy(context);
        }
        return result->getPointerTo();
    }
}

llvm::StructType *IrGenerator::getOrCreateComplexType(const ComplexType &type) {
    auto *result = llvmModule.getTypeByName(type.type.typeName);
    if (result != nullptr) {
        return result;
    }
    std::vector<llvm::Type *> elements = {};
    for (const auto &member : type.members) {
        elements.push_back(getType(member.type));
    }
    return llvm::StructType::create(context, elements, type.type.typeName);
}

llvm::StructType *IrGenerator::getStringType() {
    auto *type = llvmModule.getTypeByName("string");
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
    log.debug("Created Integer");
}

void IrGenerator::visitFloatNode(FloatNode *node) {
    nodesToValues[node] = llvm::ConstantFP::get(context, llvm::APFloat(node->getValue()));
    log.debug("Created Float");
}

void IrGenerator::visitBoolNode(BoolNode *node) {
    nodesToValues[node] = llvm::ConstantInt::get(context, llvm::APInt(1, static_cast<uint64_t>(node->getValue())));
    log.debug("Created Bool");
}

void IrGenerator::visitStringNode(StringNode *node) {
    // two options:
    //      generate code that inits string
    //      call function that inits string
    // going with second option for now

    const std::string stringValue = node->getValue();
    unsigned int numCharacters = stringValue.size();
    auto *data = builder.CreateGlobalStringPtr(stringValue, "str");
    auto *size = llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), numCharacters);
    auto *maxSize = llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), numCharacters);

    std::vector<llvm::Value *> args = {data, size, maxSize};
    auto value = createStdLibCall("createString", args);

    if (currentDestination == nullptr) {
        currentDestination = createEntryBlockAlloca(getStringType()->getPointerTo(), "localString");
    }
    builder.CreateStore(value, currentDestination);

    nodesToValues[node] = currentDestination;
    currentDestination = nullptr;

    currentScope().cleanUpFunctions.emplace_back([this, node]() {
        llvm::Value *loadedValue = builder.CreateLoad(nodesToValues[node]);
        std::vector<llvm::Value *> args = {};
        args.push_back(loadedValue);
        createStdLibCall("deleteString", args);
    });

    log.debug("Created String");
}

void IrGenerator::visitTypeDeclarationNode(TypeDeclarationNode *node) {
    auto *functionDef = getOrCreateFunctionDefinition(node->getName(), node->getType(), {});
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry-" + node->getName(), functionDef);
    builder.SetInsertPoint(BB);

    auto *complexType = getType(node->getType());
    auto dataLayout = llvmModule.getDataLayout();
    auto typeSize = dataLayout.getTypeAllocSize(complexType);
    auto fixedTypeSize = typeSize.getFixedSize();
    std::vector<llvm::Value *> args = {llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), fixedTypeSize)};
    auto *result = createStdLibCall("malloc", args);
    auto *castedResult = builder.CreateBitOrPointerCast(result, complexType);

    for (int i = 0; i < node->getMembers().size(); i++) {
        auto member = node->getMembers()[i];

        auto *memberType = getType(member->getDefinition()->getType());

        auto llvmT = getType(node->getType());
        auto elementType = llvmT->getPointerElementType();

        llvm::Value *indexOfBaseVariable = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
        llvm::Value *indexOfMember = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), i);
        std::vector<llvm::Value *> indices = {indexOfBaseVariable, indexOfMember};

        auto address = builder.CreateInBoundsGEP(elementType, castedResult, indices, "memberAccess");
        if (!ast::isSimpleDataType(member->getDefinition()->getType())) {
            auto subTypeFuncDef = getOrCreateFunctionDefinition(member->getDefinition()->getType().typeName,
                                                                member->getDefinition()->getType(), {});
            auto funcResult = builder.CreateCall(subTypeFuncDef, {});
            builder.CreateStore(funcResult, address);
        } else if (member->getDefinition()->getType() == ast::DataType(ast::SimpleDataType::STRING)) {
            int defaultSize = 32;
            auto data = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(context));
            auto size = llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), 0);
            auto maxSize = llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), defaultSize);

            std::vector<llvm::Value *> createStringArgs = {data, size, maxSize};
            auto funcResult = createStdLibCall("createString", createStringArgs);
            builder.CreateStore(funcResult, address);
        } else {
            llvm::Value *value = nullptr;
            switch (ast::toSimpleDataType(member->getDefinition()->getType())) {
            case ast::SimpleDataType::INT:
                value = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
                break;
            case ast::SimpleDataType::FLOAT:
                value = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 0);
                break;
            case ast::SimpleDataType::BOOL:
                value = llvm::ConstantInt::get(llvm::Type::getInt1Ty(context), 0);
                break;
            }
            if (value != nullptr) {
                builder.CreateStore(value, address);
            } else {
                logError("Failed to store default value for simple data type during complex type initialization");
            }
        }
    }

    builder.CreateRet(castedResult);
}
