#include "FunctionFinder.h"

#include "../nodes/AllNodes.h"

void FunctionFinder::visitSequenceNode(SequenceNode *node) {
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
}

void FunctionFinder::visitStatementNode(StatementNode *node) { node->getChild()->accept(this); }

void FunctionFinder::visitFunctionNode(FunctionNode *node) {
    FunctionSignature funcSig = {};
    funcSig.name = node->getName();
    funcSig.returnType = node->getReturnType();
    for (auto &argument : node->getArguments()) {
        FunctionArgument funcArg = {};
        funcArg.name = argument->getName();
        funcArg.type = argument->getType();
        funcSig.arguments.push_back(funcArg);
    }
    functions.push_back(funcSig);
}

std::vector<FunctionSignature> FunctionFinder::run(AstNode *rootNode) {
    rootNode->accept(this);
    return functions;
}
