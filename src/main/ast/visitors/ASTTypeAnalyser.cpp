#include "ASTTypeAnalyser.h"

#include <iostream>

#include "../nodes/AllNodes.h"
#include "../../Utils.h"

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
    if ((node->getType() == BinaryOperationNode::BinaryOperationType::ADDITION ||
         node->getType() == BinaryOperationNode::BinaryOperationType::SUBTRACTION ||
         node->getType() == BinaryOperationNode::BinaryOperationType::MULTIPLICATION ||
         node->getType() == BinaryOperationNode::BinaryOperationType::DIVISION) &&
        leftType == rightType) {
        typeMap[node] = leftType;
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

void ASTTypeAnalyser::visitIfStatementNode(IfStatementNode * /*node*/) {
    NOT_IMPLEMENTED
}

void analyseTypes(AstNode *root) {
    if (root == nullptr) {
        return;
    }
    auto analyser = new ASTTypeAnalyser();
    root->accept(analyser);
}
