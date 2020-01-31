#include "TypeAnalyser.h"

#include <iostream>

#include "../../../Utils.h"
#include "../nodes/AllNodes.h"

void TypeAnalyser::visitFunctionNode(FunctionNode *node) {
    for (auto &argument : node->getArguments()) {
        argument->accept(this);
    }
    if (!node->isExternal()) {
        node->getBody()->accept(this);
    }
}

void TypeAnalyser::visitCallNode(CallNode *node) {
    auto result = functionResolver.resolveFunction(module, node->getName());
    if (!result.functionExists) {
        std::cerr << "TypeAnalyser: Undefined function " << node->getName() << std::endl;
        return;
    }
    for (const auto arg : node->getArguments()) {
        arg->accept(this);
    }
    typeMap[node] = result.signature.returnType;
}

void TypeAnalyser::visitVariableNode(VariableNode *node) {
    const auto &itr = variableMap.find(node->getName());
    if (itr == variableMap.end()) {
        std::cerr << "TypeAnalyser: Undefined variable " << node->getName() << std::endl;
        return;
    }
    typeMap[node] = itr->second;
}

void TypeAnalyser::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    typeMap[node] = node->getType();
    variableMap[node->getName()] = node->getType();
}

void TypeAnalyser::visitBinaryOperationNode(BinaryOperationNode *node) {
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
    std::cerr << "TypeAnalyser: Binary operation type mismatch: " << to_string(node->getAstNodeType()) << std::endl;
}

void TypeAnalyser::visitUnaryOperationNode(UnaryOperationNode *node) {
    node->getChild()->accept(this);
    if (node->getType() == UnaryOperationNode::UnaryOperationType::NOT &&
        typeMap[node->getChild()] == ast::DataType::BOOL) {
        typeMap[node] = ast::DataType::BOOL;
        return;
    }
    // TODO(henne): unary operators can also be of other types than bool, we need to add support for that as well
    std::cerr << "TypeAnalyser: Unary operation type mismatch: " << to_string(node->getAstNodeType()) << std::endl;
}

void TypeAnalyser::visitAssignmentNode(AssignmentNode *node) {
    node->getRight()->accept(this);
    node->getLeft()->accept(this);
    ast::DataType leftType = typeMap[node->getLeft()];
    ast::DataType rightType = typeMap[node->getRight()];
    if (leftType != rightType) {
        std::cerr << "TypeAnalyser: Assignment type mismatch: " << to_string(leftType) << " = " << to_string(rightType)
                  << std::endl;
        return;
    }

    typeMap[node] = leftType;
}

void TypeAnalyser::visitSequenceNode(SequenceNode *node) {
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
}

void TypeAnalyser::visitStatementNode(StatementNode *node) {
    if (node->getChild() == nullptr) {
        return;
    }
    node->getChild()->accept(this);
    typeMap[node] = typeMap[node->getChild()];
}

void TypeAnalyser::visitBoolNode(BoolNode *node) { typeMap[node] = ast::DataType::BOOL; }

void TypeAnalyser::visitFloatNode(FloatNode *node) { typeMap[node] = ast::DataType::FLOAT; }

void TypeAnalyser::visitIntegerNode(IntegerNode *node) { typeMap[node] = ast::DataType::INT; }

void TypeAnalyser::visitStringNode(StringNode *node) { typeMap[node] = ast::DataType::STRING; }

void TypeAnalyser::visitIfStatementNode(IfStatementNode *node) {
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

void TypeAnalyser::visitForStatementNode(ForStatementNode *node) {
    if (node->getInit() != nullptr) {
        node->getInit()->accept(this);
    }

    node->getCondition()->accept(this);
    if (typeMap[node->getCondition()] != ast::DataType::BOOL) {
        std::cerr << "For condition is not of type bool" << std::endl;
        return;
    }

    if (node->getUpdate() != nullptr) {
        node->getUpdate()->accept(this);
    }

    if (node->getBody() != nullptr) {
        node->getBody()->accept(this);
    }
}

std::unordered_map<AstNode *, ast::DataType> TypeAnalyser::run(AstNode *root) {
    if (root == nullptr) {
        return {};
    }
    root->accept(this);
    return typeMap;
}
