#include "IrGenerator.h"

void IrGenerator::visitVariableNode(VariableNode *node) {
    LOG("Enter Variable")

    auto value = findVariable(node->getName());
    if (value == nullptr) {
        return logError("Undefined variable '" + node->getName() + "'");
    }

    nodesToValues[node] = builder.CreateLoad(value, node->getName());

    LOG("Exit Variable")
}

void IrGenerator::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    LOG("Enter VariableDefinition")

    llvm::Type *type = getType(node->getType());
    std::string &name = node->getName();
    llvm::Value *value = nullptr;
    if (isGlobalScope) {
        value = module.getOrInsertGlobal(name, type);
        module.getNamedGlobal(name)->setDSOLocal(true);
        module.getNamedGlobal(name)->setInitializer(getInitializer(node->getType()));
    } else {
        value = createEntryBlockAlloca(type, name);
    }
    currentScope()[name] = value;
    nodesToValues[node] = value;

    LOG("Exit VariableDefinition")
}
