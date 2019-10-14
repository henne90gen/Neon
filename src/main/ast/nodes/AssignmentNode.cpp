#include "AssignmentNode.h"

#include "../ASTVisitor.h"

void AssignmentNode::accept(ASTVisitor *v) { v->visitAssignmentNode(this); }

auto AssignmentNode::getLeft() -> AstNode * { return left; }

void AssignmentNode::setLeft(AstNode *left) { this->left = left; }

auto AssignmentNode::getRight() -> AstNode * { return right; }

void AssignmentNode::setRight(AstNode *right) { this->right = right; }
