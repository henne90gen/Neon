#include "IrGenerator.h"

#include "util/Utils.h"

void IrGenerator::visitVariableNode(VariableNode *node) {
    log.debug("Enter Variable");

    auto value = findVariable(node->getName());
    if (value == nullptr) {
        return logError("Undefined variable '" + node->getName() + "'");
    }

    if (node->isArrayAccess()) {
        node->getArrayIndex()->accept(this);
        llvm::Value *indexOfArray = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
        auto arrayIndex = nodesToValues[node->getArrayIndex()];
        std::vector<llvm::Value *> indices = {indexOfArray, arrayIndex};
        auto elementPtr = builder.CreateInBoundsGEP(value, indices);
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

    llvm::Value *value;
    if (isGlobalScope) {
        value = llvmModule.getOrInsertGlobal(name, type);
        llvmModule.getNamedGlobal(name)->setDSOLocal(true);
        llvmModule.getNamedGlobal(name)->setInitializer(
              getInitializer(node->getType(), node->isArray(), node->getArraySize()));
    } else {
        value = createEntryBlockAlloca(type, name);
        if (node->isArray()) {
            auto dataLayout = llvmModule.getDataLayout();
            auto typeSize = dataLayout.getTypeAllocSize(getType(node->getType()));
            auto fixedTypeSize = typeSize.getFixedSize();
            // FIXME this only works because our type system doesn't really know about array types
            fixedTypeSize *= node->getArraySize();
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
    nodesToValues[node] = value;

    log.debug("Exit VariableDefinition");
}

void IrGenerator::visitAssignmentNode(AssignmentNode *node) {
    log.debug("Enter Assignment");

    llvm::Value *dest;
    if (node->getLeft()->getAstNodeType() == ast::NodeType::VARIABLE_DEFINITION) {
        // generate variable definition
        node->getLeft()->accept(this);
        dest = nodesToValues[node->getLeft()];
    } else if (node->getLeft()->getAstNodeType() == ast::NodeType::VARIABLE) {
        // lookup the variable to save into
        auto variable = dynamic_cast<VariableNode *>(node->getLeft());
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
        // TODO this is a hack!
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
    log.debug("Enter MemberAccess");

    auto variables = node->linearizeAccessTree();
    if (variables.empty()) {
        return logError("Failed to linearize MemberAccess tree");
    }

    llvm::Value *result = findVariable(variables[0]->getName());

    llvm::Value *indexOfBaseVariable = llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0);
    std::vector<llvm::Value *> indices = {indexOfBaseVariable};

    ast::DataType previousType = ast::DataType();
    for (int i = 1; i < variables.size(); i++) {
        previousType = typeResolver.getTypeOf(module, variables[i - 1]);

        auto resolveResult = typeResolver.resolveType(module, previousType);
        if (!resolveResult.typeExists) {
            return logError("Could not resolve type: " + to_string(previousType));
        }

        int memberIndex = -1;
        bool isComplexType = false;
        for (int j = 0; j < resolveResult.complexType.members.size(); j++) {
            if (resolveResult.complexType.members[j].name == variables[i]->getName()) {
                memberIndex = j;
                isComplexType = !ast::isSimpleDataType(resolveResult.complexType.members[j].type);
                break;
            }
        }
        if (memberIndex == -1) {
            return logError("Could not find member: " + variables[i]->getName());
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

    nodesToValues[node] = result;

    log.debug("Exit MemberAccess");
}
