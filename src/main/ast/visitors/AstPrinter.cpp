#include "AstPrinter.h"

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

void AstPrinter::visitFunctionNode(FunctionNode *node) {
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

void AstPrinter::visitVariableNode(VariableNode *node) {
    indent(indentation);
    std::cout << "VariableNode(name='" << node->getName() << "', isArrayAccess='" << node->isArrayAccess()
              << "', arrayIndex='" << node->getArrayIndex() << "')" << std::endl;
}

void AstPrinter::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    indent(indentation);
    std::cout << "VariableDefinitionNode(type='" << node->getType() << "', name='" << node->getName() << "', isArray='"
              << node->isArray() << "', arraySize='" << node->getArraySize() << "')" << std::endl;
}

void AstPrinter::visitBinaryOperationNode(BinaryOperationNode *node) {
    indent(indentation);
    std::string operationType = printBinaryOperationType(node->getType());
    std::cout << "BinaryOperationNode(hasLeft=" << (node->getLeft() != nullptr)
              << ", hasRight=" << (node->getRight() != nullptr) << ", type=" << operationType << ")" << std::endl;
    indentation++;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indentation--;
}

void AstPrinter::visitUnaryOperationNode(UnaryOperationNode *node) {
    indent(indentation);
    std::string operationType = printUnaryOperationType(node->getType());
    std::cout << "UnaryOperationNode(hasChild=" << (node->getChild() != nullptr) << ", type=" << operationType << ")"
              << std::endl;
    indentation++;
    node->getChild()->accept(this);
    indentation--;
}

void AstPrinter::visitSequenceNode(SequenceNode *node) {
    indent(indentation);
    std::cout << "SequenceNode(size=" << node->getChildren().size() << ")" << std::endl;
    indentation++;
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
    indentation--;
}

void AstPrinter::visitStatementNode(StatementNode *node) {
    indent(indentation);
    std::cout << "StatementNode(isReturnStatement=" << node->isReturnStatement() << ")" << std::endl;
    indentation++;
    if (node->getChild() != nullptr) {
        node->getChild()->accept(this);
    }
    indentation--;
}

void AstPrinter::visitFloatNode(FloatNode *node) {
    indent(indentation);
    std::cout << "FloatNode(value=" << node->getValue() << ")" << std::endl;
}

void AstPrinter::visitIntegerNode(IntegerNode *node) {
    indent(indentation);
    std::cout << "IntegerNode(value=" << node->getValue() << ")" << std::endl;
}

void AstPrinter::visitBoolNode(BoolNode *node) {
    indent(indentation);
    std::cout << "BoolNode(value=" << node->getValue() << ")" << std::endl;
}

void AstPrinter::visitAssignmentNode(AssignmentNode *node) {
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

void AstPrinter::visitCallNode(CallNode *node) {
    indent(indentation);
    std::cout << "CallNode(name=" << node->getName() << ", numArguments=" << node->getArguments().size() << ")"
              << std::endl;
    indentation++;
    for (auto argument : node->getArguments()) {
        argument->accept(this);
    }
    indentation--;
}

void AstPrinter::visitIfStatementNode(IfStatementNode *node) {
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

void AstPrinter::visitForStatementNode(ForStatementNode *node) {
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

void AstPrinter::run() {
    if (module->root == nullptr) {
        std::cerr << "Could not print AST (nullptr)." << std::endl;
        return;
    }

    module->root->accept(this);
}
