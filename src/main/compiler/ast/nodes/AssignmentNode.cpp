#include "AssignmentNode.h"

#include "../AstVisitor.h"

void AssignmentNode::accept(AstVisitor *v) { v->visitAssignmentNode(this); }

AstNode *AssignmentNode::getLeft() { return left; }

void AssignmentNode::setLeft(AstNode *left) { this->left = left; }

AstNode *AssignmentNode::getRight() { return right; }

void AssignmentNode::setRight(AstNode *right) { this->right = right; }
