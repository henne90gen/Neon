#include "ASTTypeAnalyser.h"

#include <iostream>

#include "../../Utils.h"
#include "../nodes/AllNodes.h"

void ASTTypeAnalyser::visitFunctionNode(FunctionNode *node) {
    for (auto &argument : node->getArguments()) {
        argument->accept(this);
    }
    functionMap[node->getName()] = node->getReturnType();
    if (!node->isExternal()) {
        node->getBody()->accept(this);
    }
}

void ASTTypeAnalyser::visitCallNode(CallNode *node) {
    const auto &itr = functionMap.find(node->getName());
    if (itr == functionMap.end()) {
        std::cerr << "Undefined function " << node->getName() << std::endl;
    }
    typeMap[node] = itr->second;
}

void ASTTypeAnalyser::visitVariableNode(VariableNode *node) {
    const auto &itr = variableMap.find(node->getName());
    if (itr == variableMap.end()) {
        std::cerr << "Undefined variable " << node->getName() << std::endl;
        return;
    }
    typeMap[node] = itr->second;
}

void ASTTypeAnalyser::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    typeMap[node] = node->getType();
    variableMap[node->getName()] = node->getType();
}

void ASTTypeAnalyser::visitBinaryOperationNode(BinaryOperationNode *node) {
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    auto leftType = typeMap[node->getLeft()];
    auto rightType = typeMap[node->getRight()];
    if (leftType == rightType) {
        if (node->getType() == ast::BinaryOperationType::ADDITION ||
            node->getType() == ast::BinaryOperationType::SUBTRACTION ||
            node->getType() == ast::BinaryOperationType::MULTIPLICATION ||
            node->getType() == ast::BinaryOperationType::DIVISION) {
            typeMap[node] = leftType;
        } else if (node->getType() == ast::BinaryOperationType::LESS_EQUALS ||
                   node->getType() == ast::BinaryOperationType::LESS_THAN ||
                   node->getType() == ast::BinaryOperationType::GREATER_EQUALS ||
                   node->getType() == ast::BinaryOperationType::GREATER_THAN ||
                   node->getType() == ast::BinaryOperationType::EQUALS ||
                   node->getType() == ast::BinaryOperationType::NOT_EQUALS) {
            typeMap[node] = ast::DataType::BOOL;
        }
        return;
    }
    std::cerr << "Binary operation type mismatch: " << to_string(node->getAstNodeType()) << std::endl;
}

void ASTTypeAnalyser::visitUnaryOperationNode(UnaryOperationNode *node) {
    node->getChild()->accept(this);
    if (node->getType() == UnaryOperationNode::UnaryOperationType::NOT &&
        typeMap[node->getChild()] == ast::DataType::BOOL) {
        typeMap[node] = ast::DataType::BOOL;
        return;
    }
    // TODO(henne): unary operators can also be of other types than bool, we need to add support for that as well
    std::cerr << "Unary operation type mismatch: " << to_string(node->getAstNodeType()) << std::endl;
}

void ASTTypeAnalyser::visitAssignmentNode(AssignmentNode *node) {
    node->getRight()->accept(this);
    node->getLeft()->accept(this);
    ast::DataType leftType = typeMap[node->getLeft()];
    ast::DataType rightType = typeMap[node->getRight()];
    if (leftType != rightType) {
        std::cerr << "Assignment type mismatch: " << to_string(node->getLeft()->getAstNodeType()) << " = "
                  << to_string(node->getRight()->getAstNodeType()) << std::endl;
    } else {
        typeMap[node] = leftType;
    }
}

void ASTTypeAnalyser::visitSequenceNode(SequenceNode *node) {
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
}

void ASTTypeAnalyser::visitStatementNode(StatementNode *node) {
    if (node->getChild() == nullptr) {
        return;
    }
    node->getChild()->accept(this);
    typeMap[node] = typeMap[node->getChild()];
}

void ASTTypeAnalyser::visitFloatNode(FloatNode *node) { typeMap[node] = ast::DataType::FLOAT; }

void ASTTypeAnalyser::visitIntegerNode(IntegerNode *node) { typeMap[node] = ast::DataType::INT; }

void ASTTypeAnalyser::visitBoolNode(BoolNode *node) { typeMap[node] = ast::DataType::BOOL; }

void ASTTypeAnalyser::visitIfStatementNode(IfStatementNode *node) {
    node->getCondition()->accept(this);
    if (typeMap[node->getCondition()] != ast::DataType::BOOL) {
        std::cerr << "If condition is not of type bool" << std::endl;
        return;
    }
    if (node->getIfBody() != nullptr) {
        node->getIfBody()->accept(this);
    }
    if (node->getElseBody() != nullptr) {
        node->getElseBody()->accept(this);
    }
}

void analyseTypes(AstNode *root) {
    if (root == nullptr) {
        return;
    }
    auto analyser = new ASTTypeAnalyser();
    root->accept(analyser);
}
