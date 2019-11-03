#include "ASTPrinter.h"

#include <iostream>

#include "../../Utils.h"
#include "../nodes/AllNodes.h"

void indent(int indentation) {
    for (int i = 0; i < indentation; i++) {
        std::cout << "  ";
    }
}

std::string printUnaryOperationType(UnaryOperationNode::UnaryOperationType type) {
    if (type == UnaryOperationNode::NOT) {
        return "NOT";
    }
    return "ERROR";
}

std::string printBinaryOperationType(ast::BinaryOperationType type) {
    switch (type) {
    case ast::BinaryOperationType::ADDITION:
        return "ADDITION";
    case ast::BinaryOperationType::MULTIPLICATION:
        return "MULTIPLICATION";
    case ast::BinaryOperationType::SUBTRACTION:
        return "SUBTRACTION";
    case ast::BinaryOperationType::DIVISION:
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
    if (!node->isExternal()) {
        node->getBody()->accept(this);
    }
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

void ASTPrinter::visitIfStatementNode(IfStatementNode *node) {
    indent(indentation);
    std::cout << "IfStatementNode()" << std::endl;
    indentation++;
    node->getCondition()->accept(this);
    if (node->getIfBody() != nullptr) {
        node->getIfBody()->accept(this);
    }
    if (node->getElseBody() != nullptr) {
        node->getElseBody()->accept(this);
    }
    indentation--;
}

void ASTPrinter::visitForStatementNode(ForStatementNode *node) {
    indent(indentation);
    std::cout << "ForStatementNode()" << std::endl;
    indentation++;
    if (node->getInit() != nullptr) {
        node->getInit()->accept(this);
    }
    if (node->getCondition() != nullptr) {
        node->getCondition()->accept(this);
    }
    if (node->getUpdate() != nullptr) {
        node->getUpdate()->accept(this);
    }
    if (node->getBody() != nullptr) {
        node->getBody()->accept(this);
    }
    indentation--;
}

void printAst(AstNode *root) {
    if (root == nullptr) {
        std::cerr << "Could not print AST (nullptr)." << std::endl;
        return;
    }

    auto printer = new ASTPrinter();
    root->accept(printer);
}
