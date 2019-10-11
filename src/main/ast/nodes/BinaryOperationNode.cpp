#include "BinaryOperationNode.h"
#include "../ASTVisitor.h"

void BinaryOperationNode::accept(ASTVisitor *v) { v->visitBinaryOperationNode(this); }

AstNode *BinaryOperationNode::getLeft() { return left; }

void BinaryOperationNode::setLeft(AstNode *left) { this->left = left; }

AstNode *BinaryOperationNode::getRight() { return right; }

void BinaryOperationNode::setRight(AstNode *right) { this->right = right; }

BinaryOperationNode::BinaryOperationType BinaryOperationNode::getType() { return type; }
