#include "IrGenerator.h"

#include "../../Utils.h"

void IrGenerator::visitVariableNode(VariableNode *node) {
    LOG("Enter Variable")

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
        if (isPrimitiveType(typeResolver.getTypeOf(node))) {
            llvm::Value *loadedValue = builder.CreateLoad(value, node->getName());
            nodesToValues[node] = loadedValue;
        } else {
            // this directly passes the pointer, instead of loading the value first
            nodesToValues[node] = value;
        }
    }

    LOG("Exit Variable")
}

void IrGenerator::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    LOG("Enter VariableDefinition")

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
    }

    currentScope().definedVariables[name] = value;
    nodesToValues[node] = value;

    LOG("Exit VariableDefinition")
}

void IrGenerator::visitAssignmentNode(AssignmentNode *node) {
    LOG("Enter Assignment")

    llvm::Value *dest;
    if (node->getLeft()->getAstNodeType() == ast::NodeType::VARIABLE_DEFINITION) {
        // generate variable definition
        node->getLeft()->accept(this);
        dest = nodesToValues[node->getLeft()];
    } else {
        // lookup the variable to save into
        auto variable = dynamic_cast<VariableNode *>(node->getLeft());
        dest = findVariable(variable->getName());
        if (variable->isArrayAccess()) {
            variable->getArrayIndex()->accept(this);

            // This first accesses the array
            llvm::Value *indexOfArray = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
            // and then indexes into the array
            llvm::Value *indexInsideArray = nodesToValues[variable->getArrayIndex()];
            // for multi dimensional array access
            std::vector<llvm::Value *> indices = {indexOfArray, indexInsideArray};
            dest = builder.CreateInBoundsGEP(dest, indices);
        }
    }
    currentDestination = dest;

    node->getRight()->accept(this);
    llvm::Value *src = nodesToValues[node->getRight()];
    if (src == nullptr || dest == nullptr) {
        return logError("Could not create assignment.");
    }

    if (typeResolver.getTypeOf(node->getRight()) == ast::DataType(ast::SimpleDataType::STRING)) {
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

    LOG("Exit Assignment")
}
