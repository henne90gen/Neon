#include "UnaryOperationNode.h"

#include "../ASTVisitor.h"

void UnaryOperationNode::accept(ASTVisitor *v) { v->visitUnaryOperationNode(this); }

AstNode *UnaryOperationNode::getChild() { return child; }

void UnaryOperationNode::setChild(AstNode *child) { this->child = child; }

UnaryOperationNode::UnaryOperationType UnaryOperationNode::getType() { return type; }
