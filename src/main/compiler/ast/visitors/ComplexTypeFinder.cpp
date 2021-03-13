#include "ComplexTypeFinder.h"

void ComplexTypeFinder::visitSequenceNode(SequenceNode *node) {
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
}

void ComplexTypeFinder::visitStatementNode(StatementNode *node) { node->getChild()->accept(this); }

void ComplexTypeFinder::visitTypeDeclarationNode(TypeDeclarationNode *node) {
    ComplexType t = {.type = node->getType()};

    for (auto const &member : node->getMembers()) {
        ComplexTypeMember m = {.name = member->getName(), .type = member->getType()};
        t.members.push_back(m);
    }

    types.push_back(t);
}

std::vector<ComplexType> ComplexTypeFinder::run(AstNode *rootNode) {
    rootNode->accept(this);
    return types;
}
