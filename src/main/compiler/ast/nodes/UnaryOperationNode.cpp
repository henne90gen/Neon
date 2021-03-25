#include "UnaryOperationNode.h"

#include "../AstVisitor.h"

void UnaryOperationNode::accept(AstVisitor *v) { v->visitUnaryOperationNode(this); }

AstNode *UnaryOperationNode::getChild() { return child; }

void UnaryOperationNode::setChild(AstNode *child) { this->child = child; }

ast::UnaryOperationType UnaryOperationNode::getType() { return type; }
