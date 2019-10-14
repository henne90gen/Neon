#include "UnaryOperationNode.h"

#include "../ASTVisitor.h"

void UnaryOperationNode::accept(ASTVisitor *v) { v->visitUnaryOperationNode(this); }

auto UnaryOperationNode::getChild() -> AstNode * { return child; }

void UnaryOperationNode::setChild(AstNode *child) { this->child = child; }

auto UnaryOperationNode::getType() -> UnaryOperationNode::UnaryOperationType { return type; }
