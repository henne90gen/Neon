#include "ASTTestCasePrinter.h"

#include <iostream>

void ASTTestCasePrinter::printNode(AstNode *node) {
    std::cout << "        {" << indentation << ", " << to_string(node->getAstNodeType()) << "}," << std::endl;
}

void ASTTestCasePrinter::visitFunctionNode(FunctionNode *node) {
    printNode(node);
    indentation++;
    for (auto argument : node->getArguments()) {
        argument->accept(this);
    }
    if (!node->isExternal()) {
        node->getBody()->accept(this);
    }
    indentation--;
}

void ASTTestCasePrinter::visitVariableNode(VariableNode *node) { printNode(node); }

void ASTTestCasePrinter::visitVariableDefinitionNode(VariableDefinitionNode *node) { printNode(node); }

void ASTTestCasePrinter::visitBinaryOperationNode(BinaryOperationNode *node) {
    printNode(node);
    indentation++;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indentation--;
}

void ASTTestCasePrinter::visitUnaryOperationNode(UnaryOperationNode *node) {
    printNode(node);
    indentation++;
    node->getChild()->accept(this);
    indentation--;
}

void ASTTestCasePrinter::visitAssignmentNode(AssignmentNode *node) {
    printNode(node);
    indentation++;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indentation--;
}

void ASTTestCasePrinter::visitSequenceNode(SequenceNode *node) {
    printNode(node);
    indentation++;
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
    indentation--;
}

void ASTTestCasePrinter::visitStatementNode(StatementNode *node) {
    printNode(node);
    indentation++;
    node->getChild()->accept(this);
    indentation--;
}

void ASTTestCasePrinter::visitFloatNode(FloatNode *node) { printNode(node); }

void ASTTestCasePrinter::visitIntegerNode(IntegerNode *node) { printNode(node); }

void ASTTestCasePrinter::visitBoolNode(BoolNode *node) { printNode(node); }

void ASTTestCasePrinter::visitCallNode(CallNode *node) {
    printNode(node);
    indentation++;
    for (auto argument : node->getArguments()) {
        argument->accept(this);
    }
    indentation--;
}

void ASTTestCasePrinter::visitIfStatementNode(IfStatementNode *node) {
    printNode(node);
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

void printAstTestCase(const Program &program, AstNode *root) {
    if (root == nullptr) {
        std::cerr << "Could not print AST test case (nullptr)." << std::endl;
        return;
    }

    std::cout << std::endl;
    std::cout << "TEST_CASE(\"ASTGenerator can handle '" << program.toString() << "'\") {" << std::endl;
    std::cout << "    std::vector<AstNodeSpec> spec = {" << std::endl;

    auto testCasePrinter = new ASTTestCasePrinter();
    root->accept(testCasePrinter);

    std::cout << "    };" << std::endl;
    std::cout << "    std::vector<std::string> program = {\"" << program.toString() << "\"};" << std::endl;
    std::cout << "    assertProgramCreatesAst(program, spec);" << std::endl;
    std::cout << "}" << std::endl;
    std::cout << std::endl;
}