#include "TypeAnalyzer.h"

#include <iostream>

#include "../../../Utils.h"
#include "../nodes/AllNodes.h"

void TypeAnalyzer::visitFunctionNode(FunctionNode *node) {
    for (auto &argument : node->getArguments()) {
        argument->accept(this);
    }
    if (!node->isExternal()) {
        node->getBody()->accept(this);
    }
}

void TypeAnalyzer::visitCallNode(CallNode *node) {
    auto result = functionResolver.resolveFunction(module, node->getName());
    if (!result.functionExists) {
        std::cerr << "TypeAnalyzer: Undefined function " << node->getName() << std::endl;
        return;
    }
    for (const auto arg : node->getArguments()) {
        arg->accept(this);
    }
    typeMap[node] = result.signature.returnType;
}

void TypeAnalyzer::visitVariableNode(VariableNode *node) {
    const auto &itr = variableMap.find(node->getName());
    if (itr == variableMap.end()) {
        std::cerr << "TypeAnalyzer: Undefined variable " << node->getName() << std::endl;
        return;
    }
    typeMap[node] = itr->second;
}

void TypeAnalyzer::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    typeMap[node] = node->getType();
    variableMap[node->getName()] = node->getType();
}

void TypeAnalyzer::visitBinaryOperationNode(BinaryOperationNode *node) {
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
            typeMap[node] = ast::DataType(ast::SimpleDataType::BOOL);
        }
        return;
    }
    std::cerr << "TypeAnalyzer: Binary operation type mismatch: " << to_string(node->getAstNodeType()) << std::endl;
}

void TypeAnalyzer::visitUnaryOperationNode(UnaryOperationNode *node) {
    node->getChild()->accept(this);
    if (node->getType() == UnaryOperationNode::UnaryOperationType::NOT &&
        typeMap[node->getChild()] == ast::DataType(ast::SimpleDataType::BOOL)) {
        typeMap[node] = ast::DataType(ast::SimpleDataType::BOOL);
        return;
    }
    // TODO(henne): unary operators can also be of other types than bool, we need to add support for that as well
    std::cerr << "TypeAnalyzer: Unary operation type mismatch: " << to_string(node->getAstNodeType()) << std::endl;
}

void TypeAnalyzer::visitAssignmentNode(AssignmentNode *node) {
    node->getRight()->accept(this);
    node->getLeft()->accept(this);
    ast::DataType leftType = typeMap[node->getLeft()];
    ast::DataType rightType = typeMap[node->getRight()];
    if (leftType != rightType) {
        std::cerr << "TypeAnalyzer: Assignment type mismatch: " << to_string(leftType) << " = " << to_string(rightType)
                  << std::endl;
        return;
    }

    typeMap[node] = leftType;
}

void TypeAnalyzer::visitSequenceNode(SequenceNode *node) {
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
}

void TypeAnalyzer::visitStatementNode(StatementNode *node) {
    if (node->getChild() == nullptr) {
        return;
    }
    node->getChild()->accept(this);
    typeMap[node] = typeMap[node->getChild()];
}

void TypeAnalyzer::visitBoolNode(BoolNode *node) { typeMap[node] = ast::DataType(ast::SimpleDataType::BOOL); }

void TypeAnalyzer::visitFloatNode(FloatNode *node) { typeMap[node] = ast::DataType(ast::SimpleDataType::FLOAT); }

void TypeAnalyzer::visitIntegerNode(IntegerNode *node) { typeMap[node] = ast::DataType(ast::SimpleDataType::INT); }

void TypeAnalyzer::visitStringNode(StringNode *node) { typeMap[node] = ast::DataType(ast::SimpleDataType::STRING); }

void TypeAnalyzer::visitIfStatementNode(IfStatementNode *node) {
    node->getCondition()->accept(this);
    if (typeMap[node->getCondition()] != ast::DataType(ast::SimpleDataType::BOOL)) {
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

void TypeAnalyzer::visitForStatementNode(ForStatementNode *node) {
    if (node->getInit() != nullptr) {
        node->getInit()->accept(this);
    }

    node->getCondition()->accept(this);
    if (typeMap[node->getCondition()] != ast::DataType(ast::SimpleDataType::BOOL)) {
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

void TypeAnalyzer::visitTypeDeclarationNode(TypeDeclarationNode *node) {
}

std::unordered_map<AstNode *, ast::DataType> TypeAnalyzer::run(AstNode *root) {
    if (root == nullptr) {
        return {};
    }
    root->accept(this);
    return typeMap;
}
