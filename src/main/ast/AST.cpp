#include "AST.h"
#include <iostream>

std::string to_string(AstNode::AstNodeType type) {
    switch (type) {
    case AstNode::SEQUENCE:
        return "AstNode::SEQUENCE";
    case AstNode::STATEMENT:
        return "AstNode::STATEMENT";
    case AstNode::LITERAL:
        return "AstNode::LITERAL";
    case AstNode::UNARY_OPERATION:
        return "AstNode::UNARY_OPERATION";
    case AstNode::BINARY_OPERATION:
        return "AstNode::BINARY_OPERATION";
    case AstNode::FUNCTION:
        return "AstNode::FUNCTION";
    case AstNode::VARIABLE_DEFINITION:
        return "AstNode::VARIABLE_DEFINITION";
    case AstNode::VARIABLE:
        return "AstNode::VARIABLE";
    case AstNode::ASSIGNMENT:
        return "AstNode::ASSIGNMENT";
    case AstNode::CALL:
        return "AstNode::CALL";
    case AstNode::IF_STATEMENT:
        return "AstNode::IF_STATEMENT";
    default:
        std::cout << "Not implemented (AST.cpp - to_string)" << std::endl;
        exit(1);
        return "ERROR";
    }
}
