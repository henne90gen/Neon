#include "IrGenerator.h"

#include "../Utils.h"

void IrGenerator::visitVariableNode(VariableNode *node) {
    LOG("Enter Variable")

    auto value = findVariable(node->getName());
    if (value == nullptr) {
        return logError("Undefined variable '" + node->getName() + "'");
    }

    llvm::Value *loadedValue = builder.CreateLoad(value, node->getName());
    if (node->isArrayAccess()) {
        nodesToValues[node] = builder.CreateExtractValue(loadedValue, node->getArrayIndex());
    } else {
        nodesToValues[node] = loadedValue;
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

    llvm::Value *value = nullptr;
    if (isGlobalScope) {
        value = module.getOrInsertGlobal(name, type);
        module.getNamedGlobal(name)->setDSOLocal(true);
        module.getNamedGlobal(name)->setInitializer(
              getInitializer(node->getType(), node->isArray(), node->getArraySize()));
    } else {
        value = createEntryBlockAlloca(type, name);
    }
    currentScope()[name] = value;
    nodesToValues[node] = value;

    LOG("Exit VariableDefinition")
}

void IrGenerator::visitAssignmentNode(AssignmentNode *node) {
    LOG("Enter Assignment")

    llvm::Value *dest = nullptr;
    if (node->getLeft()->getAstNodeType() == ast::NodeType::VARIABLE_DEFINITION) {
        // only generate variable definitions
        node->getLeft()->accept(this);
        dest = nodesToValues[node->getLeft()];
    } else {
        auto variable = dynamic_cast<VariableNode *>(node->getLeft());
        dest = findVariable(variable->getName());
        if (variable->isArrayAccess()) {
            // This first accesses the array
            llvm::Value *indexOfArray = llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), 0);
            // and then indexes into the array
            llvm::Value *indexInsideArray =
                  llvm::ConstantInt::get(llvm::Type::getInt64Ty(context), variable->getArrayIndex());
            // for multi dimensional
            std::vector<llvm::Value *> indices = {indexOfArray, indexInsideArray};
            dest = builder.CreateInBoundsGEP(dest, indices);
        }
    }

    node->getRight()->accept(this);
    llvm::Value *src = nodesToValues[node->getRight()];
    if (src == nullptr || dest == nullptr) {
        return logError("Could not create assignment.");
    }
    nodesToValues[node] = builder.CreateStore(src, dest);

    print(false);

    LOG("Exit Assignment");
}
