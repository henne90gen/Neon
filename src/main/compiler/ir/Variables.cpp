#include "IrGenerator.h"

#include "util/Utils.h"

void IrGenerator::visitVariableNode(VariableNode *node) {
    log.debug("Enter Variable");

    auto *value = findVariable(node->getName());
    if (value == nullptr) {
        return logError("Undefined variable '" + node->getName() + "'");
    }

    if (node->isArrayAccess()) {
        node->getArrayIndex()->accept(this);
        llvm::Value *indexOfArray = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
        auto *arrayIndex = nodesToValues[node->getArrayIndex()];
        std::vector<llvm::Value *> indices = {indexOfArray, arrayIndex};
        auto *elementPtr = builder.CreateInBoundsGEP(value, indices);
        nodesToValues[node] = builder.CreateLoad(elementPtr);
    } else {
        if (isPrimitiveType(typeResolver.getTypeOf(module, node))) {
            llvm::Value *loadedValue = builder.CreateLoad(value, node->getName());
            nodesToValues[node] = loadedValue;
        } else {
            // this directly passes the pointer, instead of loading the value first
            nodesToValues[node] = value;
        }
    }

    log.debug("Exit Variable");
}

void IrGenerator::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    log.debug("Enter VariableDefinition");

    llvm::Type *type = getType(node->getType());
    std::string &name = node->getName();

    if (node->isArray()) {
        type = llvm::ArrayType::get(type, node->getArraySize());
    }

    llvm::Value *value = nullptr;
    if (isGlobalScope) {
        value = llvmModule.getOrInsertGlobal(name, type);
        llvmModule.getNamedGlobal(name)->setDSOLocal(true);
        llvmModule.getNamedGlobal(name)->setInitializer(
              getInitializer(node->getType(), node->isArray(), node->getArraySize()));
    } else {
        value = createEntryBlockAlloca(type, name);
        int sizeOfInt64 = 8;
        llvm::CastInst *arrayPtr =
              llvm::CastInst::CreatePointerCast(value, llvm::Type::getInt8PtrTy(context), "", builder.GetInsertBlock());
        std::vector<llvm::Value *> args = {
              arrayPtr,
              llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0),
              llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), node->getArraySize() * sizeOfInt64),
        };
        createStdLibCall("memset", args);
    }

    currentScope().definedVariables[name] = value;
    nodesToValues[node] = value;

    log.debug("Exit VariableDefinition");
}

void IrGenerator::visitAssignmentNode(AssignmentNode *node) {
    log.debug("Enter Assignment");

    llvm::Value *dest = nullptr;
    if (node->getLeft()->getAstNodeType() == ast::NodeType::VARIABLE_DEFINITION) {
        // generate variable definition
        node->getLeft()->accept(this);
        dest = nodesToValues[node->getLeft()];
    } else if (node->getLeft()->getAstNodeType() == ast::NodeType::VARIABLE) {
        // lookup the variable to save into
        auto *variable = dynamic_cast<VariableNode *>(node->getLeft());
        dest = findVariable(variable->getName());
        if (variable->isArrayAccess()) {
            variable->getArrayIndex()->accept(this);

            // This first accesses the array
            llvm::Value *indexOfArray = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
            // and then indexes into the array, for multi dimensional array access
            llvm::Value *indexInsideArray = nodesToValues[variable->getArrayIndex()];
            std::vector<llvm::Value *> indices = {indexOfArray, indexInsideArray};
            dest = builder.CreateInBoundsGEP(dest, indices);
        }
    } else if (node->getLeft()->getAstNodeType() == ast::NodeType::MEMBER_ACCESS) {
        // TODO(henne): this is a hack!
        //  setting currentDestination to 1 is used as a flag to signal to visitMemberAccessNode that we are going to
        //  write to its result
        currentDestination = reinterpret_cast<llvm::Value *>(1);
        node->getLeft()->accept(this);
        dest = nodesToValues[node->getLeft()];
    } else {
        return logError("Could not handle assignment to " + to_string(node->getLeft()->getAstNodeType()));
    }

    currentDestination = dest;
    node->getRight()->accept(this);
    currentDestination = nullptr;

    llvm::Value *src = nodesToValues[node->getRight()];
    if (src == nullptr || dest == nullptr) {
        return logError("Could not create assignment.");
    }

    if (typeResolver.getTypeOf(module, node->getRight()) == ast::DataType(ast::SimpleDataType::STRING)) {
        if (node->getLeft()->getAstNodeType() != ast::NodeType::VARIABLE_DEFINITION) {
            llvm::Value *loadedDest = builder.CreateLoad(dest);
            llvm::Value *loadedSrc = builder.CreateLoad(src);
            std::vector<llvm::Value *> args = {loadedDest, loadedSrc};
            createStdLibCall("assignString", args);
        }
        nodesToValues[node] = dest;
    } else {
        nodesToValues[node] = builder.CreateStore(src, dest);
    }

    log.debug("Exit Assignment");
}

void IrGenerator::visitMemberAccessNode(MemberAccessNode *node) {
    // FIXME implement MemberAccess again
    // TODO(henne): add nested member access
    log.debug("Enter MemberAccess");

    auto variables = node->linearizeAccessTree();
    if (variables.empty()) {
        return logError("Failed to linearize MemberAccess tree");
    }

    const ast::DataType baseType = typeResolver.getTypeOf(module, variables[0]);

    auto resolveResult = typeResolver.resolveType(module, baseType);
    if (!resolveResult.typeExists) {
        return logError("Could not resolve type: " + to_string(baseType));
    }

    int memberIndex = -1;
    for (int i = 0; i < resolveResult.complexType.members.size(); i++) {
        if (resolveResult.complexType.members[i].name == variables[1]->getName()) {
            memberIndex = i;
            break;
        }
    }
    if (memberIndex == -1) {
        return logError("Could not find member: " + variables[1]->getName());
    }

    auto *llvmBaseType = getType(baseType);
    auto *elementType = llvmBaseType->getPointerElementType();

    auto *baseVariable = findVariable(variables[0]->getName());
    baseVariable = builder.CreateLoad(baseVariable);

    llvm::Value *indexOfBaseVariable = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    llvm::Value *indexOfMember = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), memberIndex);
    std::vector<llvm::Value *> indices = {indexOfBaseVariable, indexOfMember};

    auto *result = builder.CreateInBoundsGEP(elementType, baseVariable, indices, "memberAccess");

    if (reinterpret_cast<long>(currentDestination) != 1) {
        result = builder.CreateLoad(result, "memberAccessLoad");
    }

    nodesToValues[node] = result;

    log.debug("Exit MemberAccess");
}
