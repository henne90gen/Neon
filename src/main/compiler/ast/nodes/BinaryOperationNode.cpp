#include "BinaryOperationNode.h"

#include "../AstVisitor.h"

void BinaryOperationNode::accept(AstVisitor *v) { v->visitBinaryOperationNode(this); }

AstNode *BinaryOperationNode::getLeft() { return left; }

void BinaryOperationNode::setLeft(AstNode *left) { this->left = left; }

AstNode *BinaryOperationNode::getRight() { return right; }

void BinaryOperationNode::setRight(AstNode *right) { this->right = right; }

ast::BinaryOperationType BinaryOperationNode::getType() { return type; }

std::string BinaryOperationNode::operationToString() {
    switch (type) {
    case ast::ADDITION:
        return " + ";
    case ast::MULTIPLICATION:
        return " * ";
    case ast::SUBTRACTION:
        return " - ";
    case ast::DIVISION:
        return " / ";
    case ast::EQUALS:
        return " == ";
    case ast::NOT_EQUALS:
        return " != ";
    case ast::LESS_EQUALS:
        return " <= ";
    case ast::LESS_THAN:
        return " < ";
    case ast::GREATER_EQUALS:
        return " >= ";
    case ast::GREATER_THAN:
        return " > ";
    }
    return "not implemented";
}

std::string BinaryOperationNode::toString() { return left->toString() + operationToString() + right->toString(); }
