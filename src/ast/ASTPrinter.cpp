#include "ASTPrinter.h"

#include <iostream>

void indent(int indentation) {
    for (int i = 0; i < indentation; i++) {
        std::cout << "  ";
    }
}

std::string printUnaryOperationType(UnaryOperationNode::UnaryOperationType type) {
    switch (type) {
    case UnaryOperationNode::NOT:
        return "NOT";
    default:
        return "ERROR";
    }
}

std::string printBinaryOperationType(BinaryOperationNode::BinaryOperationType type) {
    switch (type) {
    case BinaryOperationNode::ADDITION:
        return "ADDITION";
    case BinaryOperationNode::MULTIPLICATION:
        return "MULTIPLICATION";
    case BinaryOperationNode::SUBTRACTION:
        return "SUBTRACTION";
    case BinaryOperationNode::DIVISION:
        return "DIVISION";
    default:
        return "ERROR";
    }
}

void ASTPrinter::visitFunctionNode(FunctionNode *node) {
    indent(indentation);
    std::cout << "FunctionNode(name='" << node->getName() << "', numArguments=" << node->getArguments().size() << ")"
              << std::endl;
    indentation++;
    for (auto &argument : node->getArguments()) {
        argument->accept(this);
    }
    node->getBody()->accept(this);
    indentation--;
}

void ASTPrinter::visitVariableNode(VariableNode *node) {
    indent(indentation);
    std::cout << "VariableNode(name='" << node->getName() << "')" << std::endl;
}

void ASTPrinter::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    indent(indentation);
    std::cout << "VariableDefinitionNode(type='" << node->getType() << "', name='" << node->getName() << "')"
              << std::endl;
}

void ASTPrinter::visitBinaryOperationNode(BinaryOperationNode *node) {
    indent(indentation);
    std::string operationType = printBinaryOperationType(node->getType());
    std::cout << "BinaryOperationNode(hasLeft=" << (node->getLeft() != nullptr)
              << ", hasRight=" << (node->getRight() != nullptr) << ", type=" << operationType << ")" << std::endl;
    indentation++;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indentation--;
}

void ASTPrinter::visitUnaryOperationNode(UnaryOperationNode *node) {
    indent(indentation);
    std::string operationType = printUnaryOperationType(node->getType());
    std::cout << "UnaryOperationNode(hasChild=" << (node->getChild() != nullptr) << ", type=" << operationType << ")"
              << std::endl;
    indentation++;
    node->getChild()->accept(this);
    indentation--;
}

void ASTPrinter::visitSequenceNode(SequenceNode *node) {
    indent(indentation);
    std::cout << "SequenceNode(size=" << node->getChildren().size() << ")" << std::endl;
    indentation++;
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
    indentation--;
}

void ASTPrinter::visitStatementNode(StatementNode *node) {
    indent(indentation);
    std::cout << "StatementNode(isReturnStatement=" << node->isReturnStatement() << ")" << std::endl;
    indentation++;
    if (node->getChild() != nullptr) {
        node->getChild()->accept(this);
    }
    indentation--;
}

void ASTPrinter::visitFloatNode(FloatNode *node) {
    indent(indentation);
    std::cout << "FloatNode(value=" << node->getValue() << ")" << std::endl;
}

void ASTPrinter::visitIntegerNode(IntegerNode *node) {
    indent(indentation);
    std::cout << "IntegerNode(value=" << node->getValue() << ")" << std::endl;
}

void ASTPrinter::visitBoolNode(BoolNode *node) {
    indent(indentation);
    std::cout << "BoolNode(value=" << node->getValue() << ")" << std::endl;
}

void ASTPrinter::visitAssignmentNode(AssignmentNode *node) {
    indent(indentation);
    std::cout << "AssignmentNode(hasLeft=" << (node->getLeft() != nullptr)
              << ", hasRight=" << (node->getRight() != nullptr) << ")" << std::endl;
    indentation++;
    if (node->getLeft() != nullptr) {
        node->getLeft()->accept(this);
    }
    if (node->getRight() != nullptr) {
        node->getRight()->accept(this);
    }
    indentation--;
}

void ASTPrinter::visitCallNode(CallNode *node) {
    indent(indentation);
    std::cout << "CallNode(name=" << node->getName() << ", numArguments=" << node->getArguments().size() << ")"
              << std::endl;
    indentation++;
    for (auto argument : node->getArguments()) {
        argument->accept(this);
    }
    indentation--;
}

void printAst(AstNode *root) {
    if (root == nullptr) {
        std::cout << "Could not print AST (nullptr)." << std::endl;
        return;
    }
    auto printer = new ASTPrinter();
    root->accept(printer);
}
