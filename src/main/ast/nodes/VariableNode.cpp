#include "VariableNode.h"

#include "../AstVisitor.h"

VariableNode::VariableNode(std::string name) : AstNode(ast::NodeType::VARIABLE), name(std::move(name)) {}

void VariableNode::accept(AstVisitor *v) { v->visitVariableNode(this); }

std::string &VariableNode::getName() { return name; }

bool VariableNode::isArrayAccess() { return arrayAccess; }

int VariableNode::getArrayIndex() { return this->arrayIndex; }

void VariableNode::setArrayIndex(int arrayIndex) {
    this->arrayIndex = arrayIndex;
    this->arrayAccess = true;
}
