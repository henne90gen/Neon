#include "AstNode.h"

#include <iostream>

std::string to_string(ast::NodeType type) {
    switch (type) {
    case ast::NodeType::SEQUENCE:
        return "ast::NodeType::SEQUENCE";
    case ast::NodeType::STATEMENT:
        return "ast::NodeType::STATEMENT";
    case ast::NodeType::LITERAL:
        return "ast::NodeType::LITERAL";
    case ast::NodeType::UNARY_OPERATION:
        return "ast::NodeType::UNARY_OPERATION";
    case ast::NodeType::BINARY_OPERATION:
        return "ast::NodeType::BINARY_OPERATION";
    case ast::NodeType::FUNCTION:
        return "ast::NodeType::FUNCTION";
    case ast::NodeType::VARIABLE_DEFINITION:
        return "ast::NodeType::VARIABLE_DEFINITION";
    case ast::NodeType::VARIABLE:
        return "ast::NodeType::VARIABLE";
    case ast::NodeType::ASSIGNMENT:
        return "ast::NodeType::ASSIGNMENT";
    case ast::NodeType::CALL:
        return "ast::NodeType::CALL";
    case ast::NodeType::IF_STATEMENT:
        return "ast::NodeType::IF_STATEMENT";
    case ast::NodeType::FOR_STATEMENT:
        return "ast::NodeType::FOR_STATEMENT";
    case ast::NodeType::IMPORT:
        return "ast::NodeType::IMPORT";
    case ast::NodeType::TYPE_DECLARATION:
        return "ast::NodeType::TYPE_DECLARATION";
    case ast::NodeType::TYPE_MEMBER:
        return "ast::NodeType::TYPE_MEMBER";
    case ast::NodeType::MEMBER_ACCESS:
        return "ast::NodeType::MEMBER_ACCESS";
    case ast::NodeType::ASSERT:
        return "ast::NodeType::ASSERT";
    case ast::NodeType::COMMENT:
        return "ast::NodeType::COMMENT";
    default:
        std::cout << "Not implemented (AstNode.cpp:to_string)" << std::endl;
        exit(1);
        return "ERROR";
    }
}

std::vector<VariableNode *> MemberAccessNode::linearize_access_tree() {
    std::vector<VariableNode *> variables = {};
    std::vector<AstNode *> stack = {right, left};
    while (!stack.empty()) {
        auto *current = stack.back();
        stack.pop_back();
        if (current->type == ast::NodeType::VARIABLE) {
            variables.push_back(reinterpret_cast<VariableNode *>(current));
            continue;
        }
        if (current->type == ast::NodeType::MEMBER_ACCESS) {
            auto *ma = reinterpret_cast<MemberAccessNode *>(current);
            stack.push_back(ma->right);
            stack.push_back(ma->left);
        } else {
            // empty return value signals an error
            return {};
        }
    }
    return variables;
}

std::string BinaryOperationNode::operation_string() {
    switch (type) {
    case ast::BinaryOperationType::ADDITION:
        return " + ";
    case ast::BinaryOperationType::MULTIPLICATION:
        return " * ";
    case ast::BinaryOperationType::SUBTRACTION:
        return " - ";
    case ast::BinaryOperationType::DIVISION:
        return " / ";
    case ast::BinaryOperationType::EQUALS:
        return " == ";
    case ast::BinaryOperationType::NOT_EQUALS:
        return " != ";
    case ast::BinaryOperationType::LESS_EQUALS:
        return " <= ";
    case ast::BinaryOperationType::LESS_THAN:
        return " < ";
    case ast::BinaryOperationType::GREATER_EQUALS:
        return " >= ";
    case ast::BinaryOperationType::GREATER_THAN:
        return " > ";
    case ast::BinaryOperationType::AND:
        return " and ";
    case ast::BinaryOperationType::OR:
        return " or ";
    }
    return "not implemented";
}

std::string to_string(AstNode *node) { return "not implemented"; }

bool VariableNode::is_array_access() const { return arrayIndex != nullptr; }

bool VariableDefinitionNode::is_array() const { return arraySize > 0; }
