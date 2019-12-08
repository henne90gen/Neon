#include "FunctionFinder.h"

#include "../nodes/AllNodes.h"

void FunctionFinder::visitSequenceNode(SequenceNode *node) {
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
}

void FunctionFinder::visitStatementNode(StatementNode *node) { node->getChild()->accept(this); }

void FunctionFinder::visitFunctionNode(FunctionNode *node) {
    functions.emplace_back(node->getName(), node->getReturnType());
}

std::vector<std::pair<std::string, ast::DataType>> FunctionFinder::run(Module *module) {
    module->root->accept(this);
    return functions;
}
