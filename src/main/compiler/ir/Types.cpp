#include "IrGenerator.h"

const int NUM_BITS_OF_INT = 64;

llvm::Type *IrGenerator::getType(const ast::DataType &type) {
    bool isSimpleType = ast::isSimpleDataType(type);
    if (isSimpleType) {
        ast::SimpleDataType simpleDataType = toSimpleDataType(type);
        switch (simpleDataType) {
        case ast::SimpleDataType::VOID:
            return llvm::Type::getVoidTy(context);
        case ast::SimpleDataType::INTEGER:
            return llvm::Type::getInt64Ty(context);
        case ast::SimpleDataType::FLOAT:
            return llvm::Type::getDoubleTy(context);
        case ast::SimpleDataType::BOOLEAN:
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
    auto *result = llvm::StructType::getTypeByName(context, type.type.typeName);
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
    auto *type = llvm::StructType::getTypeByName(context, "string");
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

void IrGenerator::visitLiteralNode(LiteralNode *node) {
    switch (node->type) {
    case LiteralType::BOOL:
        nodesToValues[AST_NODE(node)] = llvm::ConstantInt::get(context, llvm::APInt(1, static_cast<uint64_t>(node->b)));
        log.debug("Created Bool");
        break;
    case LiteralType::INTEGER:
        nodesToValues[AST_NODE(node)] = llvm::ConstantInt::get(context, llvm::APInt(NUM_BITS_OF_INT, node->i));
        log.debug("Created Integer");
        break;
    case LiteralType::FLOAT:
        nodesToValues[AST_NODE(node)] = llvm::ConstantFP::get(context, llvm::APFloat(node->d));
        log.debug("Created Float");
        break;
    case LiteralType::STRING:
        visitStringNode(node);
        break;
    }
}

void IrGenerator::visitStringNode(LiteralNode *node) {
    // two options:
    //      generate code that inits string
    //      call function that inits string
    // going with second option for now

    const std::string stringValue = node->s;
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

    nodesToValues[AST_NODE(node)] = currentDestination;
    currentDestination = nullptr;

    currentScope().cleanUpFunctions.emplace_back([this, node]() {
        llvm::Value *loadedValue = builder.CreateLoad(nodesToValues[AST_NODE(node)]);
        std::vector<llvm::Value *> args = {};
        args.push_back(loadedValue);
        createStdLibCall("deleteString", args);
    });

    log.debug("Created String");
}

void IrGenerator::visitTypeDeclarationNode(TypeDeclarationNode *node) {
    auto *functionDef = getOrCreateFunctionDefinition(node->name, node->type(), {});
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry-" + node->name, functionDef);
    builder.SetInsertPoint(BB);

    auto *complexType = getType(node->type());
    auto dataLayout = llvmModule.getDataLayout();
    auto typeSize = dataLayout.getTypeAllocSize(complexType);
    auto fixedTypeSize = typeSize.getFixedSize();
    std::vector<llvm::Value *> args = {llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), fixedTypeSize)};
    auto *result = createStdLibCall("malloc", args);
    auto *castedResult = builder.CreateBitOrPointerCast(result, complexType);

    for (int i = 0; i < node->members.size(); i++) {
        auto member = node->members[i];

        auto *memberType = getType(member->variable_definition->type);

        auto llvmT = getType(node->type());
        auto elementType = llvmT->getPointerElementType();

        llvm::Value *indexOfBaseVariable = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
        llvm::Value *indexOfMember = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), i);
        std::vector<llvm::Value *> indices = {indexOfBaseVariable, indexOfMember};

        auto address = builder.CreateInBoundsGEP(elementType, castedResult, indices, "memberAccess");
        if (!ast::isSimpleDataType(member->variable_definition->type)) {
            auto subTypeFuncDef = getOrCreateFunctionDefinition(member->variable_definition->type.typeName,
                                                                member->variable_definition->type, {});
            auto funcResult = builder.CreateCall(subTypeFuncDef, {});
            builder.CreateStore(funcResult, address);
        } else if (member->variable_definition->type == ast::DataType(ast::SimpleDataType::STRING)) {
            int defaultSize = 32;
            auto data = llvm::ConstantPointerNull::get(llvm::Type::getInt8PtrTy(context));
            auto size = llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), 0);
            auto maxSize = llvm::ConstantInt::get(llvm::IntegerType::getInt64Ty(context), defaultSize);

            std::vector<llvm::Value *> createStringArgs = {data, size, maxSize};
            auto funcResult = createStdLibCall("createString", createStringArgs);
            builder.CreateStore(funcResult, address);
        } else {
            llvm::Value *value = nullptr;
            switch (ast::toSimpleDataType(member->variable_definition->type)) {
            case ast::SimpleDataType::INTEGER:
                value = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
                break;
            case ast::SimpleDataType::FLOAT:
                value = llvm::ConstantFP::get(llvm::Type::getFloatTy(context), 0);
                break;
            case ast::SimpleDataType::BOOLEAN:
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
