#include "IrGenerator.h"

#include "util/Utils.h"

void IrGenerator::visitVariableNode(VariableNode *node) {
    log.debug("Enter Variable");

    auto *value = findVariable(node->name);
    if (value == nullptr) {
        return logError("Undefined variable '" + node->name + "'");
    }

    if (node->is_array_access()) {
        visitNode(node->arrayIndex);
        llvm::Value *indexOfArray = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
        auto *arrayIndex = nodesToValues[node->arrayIndex];
        std::vector<llvm::Value *> indices = {indexOfArray, arrayIndex};
        auto *elementPtr = builder.CreateInBoundsGEP(value, indices);
        nodesToValues[AST_NODE(node)] = builder.CreateLoad(elementPtr);
    } else {
        if (isPrimitiveType(typeResolver.getTypeOf(module, AST_NODE(node)))) {
            llvm::Value *loadedValue = builder.CreateLoad(value, node->name);
            nodesToValues[AST_NODE(node)] = loadedValue;
        } else {
            // this directly passes the pointer, instead of loading the value first
            nodesToValues[AST_NODE(node)] = value;
        }
    }

    log.debug("Exit Variable");
}

void IrGenerator::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    log.debug("Enter VariableDefinition");

    llvm::Type *type = getType(node->type);
    std::string &name = node->name;

    if (node->is_array()) {
        type = llvm::ArrayType::get(type, node->arraySize);
    }

    llvm::Value *value = nullptr;
    if (isGlobalScope) {
        value = llvmModule.getOrInsertGlobal(name, type);
        llvmModule.getNamedGlobal(name)->setDSOLocal(true);
        llvmModule.getNamedGlobal(name)->setInitializer(getInitializer(node->type, node->is_array(), node->arraySize));
    } else {
        value = createEntryBlockAlloca(type, name);
        if (node->is_array()) {
            auto dataLayout = llvmModule.getDataLayout();
            auto typeSize = dataLayout.getTypeAllocSize(type);
            auto fixedTypeSize = typeSize.getFixedSize();
            // FIXME this only works because our type system doesn't really know about array types
            fixedTypeSize *= node->arraySize;
            llvm::CastInst *arrayPtr = llvm::CastInst::CreatePointerCast(value, llvm::Type::getInt8PtrTy(context), "",
                                                                         builder.GetInsertBlock());
            std::vector<llvm::Value *> args = {
                  arrayPtr,
                  llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0),
                  llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), fixedTypeSize),
            };
            createStdLibCall("memset", args);
        }
    }

    currentScope().definedVariables[name] = value;
    nodesToValues[AST_NODE(node)] = value;

    log.debug("Exit VariableDefinition");
}

void IrGenerator::visitAssignmentNode(AssignmentNode *node) {
    log.debug("Enter Assignment");

    llvm::Value *dest = nullptr;
    if (node->left->type == ast::NodeType::VARIABLE_DEFINITION) {
        // generate variable definition
        visitNode(node->left);
        dest = nodesToValues[node->left];
    } else if (node->left->type == ast::NodeType::VARIABLE) {
        // lookup the variable to save into
        auto *variable = reinterpret_cast<VariableNode *>(node->left);
        dest = findVariable(variable->name);
        if (variable->is_array_access()) {
            visitNode(variable->arrayIndex);

            // This first accesses the array
            llvm::Value *indexOfArray = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            // and then indexes into the array, for multi dimensional array access
            llvm::Value *indexInsideArray = nodesToValues[variable->arrayIndex];
            std::vector<llvm::Value *> indices = {indexOfArray, indexInsideArray};
            dest = builder.CreateInBoundsGEP(dest, indices);
        }
    } else if (node->left->type == ast::NodeType::MEMBER_ACCESS) {
        // TODO(henne): this is a hack!
        //  setting currentDestination to 1 is used as a flag to signal to visitMemberAccessNode that we are going to
        //  write to its result
        currentDestination = reinterpret_cast<llvm::Value *>(1);
        visitNode(node->left);
        dest = nodesToValues[node->left];
    } else {
        return logError("Could not handle assignment to " + to_string(node->left->type));
    }

    currentDestination = dest;
    visitNode(node->right);
    currentDestination = nullptr;

    llvm::Value *src = nodesToValues[node->right];
    if (src == nullptr || dest == nullptr) {
        return logError("Could not create assignment.");
    }

    if (typeResolver.getTypeOf(module, node->right) == ast::DataType(ast::SimpleDataType::STRING)) {
        if (node->left->type != ast::NodeType::VARIABLE_DEFINITION) {
            llvm::Value *loadedDest = builder.CreateLoad(dest);
            llvm::Value *loadedSrc = builder.CreateLoad(src);
            std::vector<llvm::Value *> args = {loadedDest, loadedSrc};
            createStdLibCall("assignString", args);
        }
        nodesToValues[AST_NODE(node)] = dest;
    } else {
        nodesToValues[AST_NODE(node)] = builder.CreateStore(src, dest);
    }

    log.debug("Exit Assignment");
}

void IrGenerator::visitMemberAccessNode(MemberAccessNode *node) {
    log.debug("Enter MemberAccess");

    auto variables = node->linearize_access_tree();
    if (variables.empty()) {
        return logError("Failed to linearize MemberAccess tree");
    }

    llvm::Value *result = findVariable(variables[0]->name);

    llvm::Value *indexOfBaseVariable = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    std::vector<llvm::Value *> indices = {indexOfBaseVariable};

    ast::DataType previousType = ast::DataType();
    for (int i = 1; i < variables.size(); i++) {
        previousType = typeResolver.getTypeOf(module, AST_NODE(variables[i - 1]));

        auto resolveResult = typeResolver.resolveType(module, previousType);
        if (!resolveResult.typeExists) {
            return logError("Could not resolve type: " + to_string(previousType));
        }

        int memberIndex = -1;
        bool isComplexType = false;
        for (int j = 0; j < resolveResult.complexType.members.size(); j++) {
            if (resolveResult.complexType.members[j].name == variables[i]->name) {
                memberIndex = j;
                isComplexType = !ast::isSimpleDataType(resolveResult.complexType.members[j].type);
                break;
            }
        }
        if (memberIndex == -1) {
            return logError("Could not find member: " + variables[i]->name);
        }

        llvm::Value *indexOfMember = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), memberIndex);
        indices.push_back(indexOfMember);

        if (isComplexType) {
            // dereference the pointer to the complex type
            auto llvmT = getType(previousType);
            auto elementType = llvmT->getPointerElementType();

            result = builder.CreateLoad(result);
            result = builder.CreateInBoundsGEP(elementType, result, indices, "memberAccess");

            indices.clear();
            indices.push_back(indexOfBaseVariable);
        }
    }

    auto llvmT = getType(previousType);
    auto elementType = llvmT->getPointerElementType();

    result = builder.CreateLoad(result);
    result = builder.CreateInBoundsGEP(elementType, result, indices, "memberAccess");

    if (reinterpret_cast<long>(currentDestination) != 1) {
        // TODO this is a hack!
        //  currentDestination is set to 1 to signal that we are going to write to its result
        result = builder.CreateLoad(result, "memberAccessLoad");
    }

    nodesToValues[AST_NODE(node)] = result;

    log.debug("Exit MemberAccess");
}

void IrGenerator::visitNode(AstNode *node) {
    switch (node->type) {
    case ast::NodeType::SEQUENCE:
        visitSequenceNode(&node->sequence);
        break;
    case ast::NodeType::STATEMENT:
        visitStatementNode(&node->statement);
        break;
    case ast::NodeType::LITERAL:
        visitLiteralNode(&node->literal);
        break;
    case ast::NodeType::UNARY_OPERATION:
        visitUnaryOperationNode(&node->unary_operation);
        break;
    case ast::NodeType::BINARY_OPERATION:
        visitBinaryOperationNode(&node->binary_operation);
        break;
    case ast::NodeType::FUNCTION:
        visitFunctionNode(&node->function);
        break;
    case ast::NodeType::CALL:
        visitCallNode(&node->call);
        break;
    case ast::NodeType::VARIABLE_DEFINITION:
        visitVariableDefinitionNode(&node->variable_definition);
        break;
    case ast::NodeType::VARIABLE:
        visitVariableNode(&node->variable);
        break;
    case ast::NodeType::ASSIGNMENT:
        visitAssignmentNode(&node->assignment);
        break;
    case ast::NodeType::IF_STATEMENT:
        visitIfStatementNode(&node->if_statement);
        break;
    case ast::NodeType::FOR_STATEMENT:
        visitForStatementNode(&node->for_statement);
        break;
    case ast::NodeType::TYPE_DECLARATION:
        visitTypeDeclarationNode(&node->type_declaration);
        break;
    case ast::NodeType::MEMBER_ACCESS:
        visitMemberAccessNode(&node->member_access);
        break;
    case ast::NodeType::ASSERT:
        visitAssertNode(&node->assert);
        break;
    case ast::NodeType::COMMENT:
    case ast::NodeType::TYPE_MEMBER:
    case ast::NodeType::IMPORT:
        break;
    }
}
