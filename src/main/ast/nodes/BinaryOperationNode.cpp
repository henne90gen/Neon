#include "BinaryOperationNode.h"

#include "../ASTVisitor.h"

void BinaryOperationNode::accept(ASTVisitor *v) { v->visitBinaryOperationNode(this); }

auto BinaryOperationNode::getLeft() -> AstNode * { return left; }

void BinaryOperationNode::setLeft(AstNode *left) { this->left = left; }

auto BinaryOperationNode::getRight() -> AstNode * { return right; }

void BinaryOperationNode::setRight(AstNode *right) { this->right = right; }

auto BinaryOperationNode::getType() -> ast::BinaryOperationType { return type; }
