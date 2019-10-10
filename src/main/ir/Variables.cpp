#include "IRGenerator.h"

void IRGenerator::visitVariableNode(VariableNode *node) {
    LOG("Enter Variable")

    if (definedVariables.find(node->getName()) == definedVariables.end()) {
        return logError("Undefined variable " + node->getName());
    }

    auto value = definedVariables[node->getName()];
    if (value == nullptr) {
        return;
    }
    nodesToValues[node] = builder.CreateLoad(value, node->getName());

    LOG("Exit Variable")
}

void IRGenerator::visitVariableDefinitionNode(VariableDefinitionNode *node) {
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
    definedVariables[name] = value;
    nodesToValues[node] = value;

    LOG("Exit VariableDefinition")
}
