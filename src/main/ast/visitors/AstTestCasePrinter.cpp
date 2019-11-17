#include "AstTestCasePrinter.h"

#include <iostream>

#include "../../Program.h"
#include "../../Utils.h"
#include "../nodes/AllNodes.h"

void AstTestCasePrinter::printNode(AstNode *node) {
    std::cout << "        {" << indentation << ", " << to_string(node->getAstNodeType()) << "}," << std::endl;
}

void AstTestCasePrinter::visitFunctionNode(FunctionNode *node) {
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

void AstTestCasePrinter::visitVariableNode(VariableNode *node) { printNode(node); }

void AstTestCasePrinter::visitVariableDefinitionNode(VariableDefinitionNode *node) { printNode(node); }

void AstTestCasePrinter::visitBinaryOperationNode(BinaryOperationNode *node) {
    printNode(node);
    indentation++;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indentation--;
}

void AstTestCasePrinter::visitUnaryOperationNode(UnaryOperationNode *node) {
    printNode(node);
    indentation++;
    node->getChild()->accept(this);
    indentation--;
}

void AstTestCasePrinter::visitAssignmentNode(AssignmentNode *node) {
    printNode(node);
    indentation++;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indentation--;
}

void AstTestCasePrinter::visitSequenceNode(SequenceNode *node) {
    printNode(node);
    indentation++;
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
    indentation--;
}

void AstTestCasePrinter::visitStatementNode(StatementNode *node) {
    printNode(node);
    indentation++;
    node->getChild()->accept(this);
    indentation--;
}

void AstTestCasePrinter::visitFloatNode(FloatNode *node) { printNode(node); }

void AstTestCasePrinter::visitIntegerNode(IntegerNode *node) { printNode(node); }

void AstTestCasePrinter::visitBoolNode(BoolNode *node) { printNode(node); }

void AstTestCasePrinter::visitCallNode(CallNode *node) {
    printNode(node);
    indentation++;
    for (auto argument : node->getArguments()) {
        argument->accept(this);
    }
    indentation--;
}

void AstTestCasePrinter::visitIfStatementNode(IfStatementNode *node) {
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

void AstTestCasePrinter::visitForStatementNode(ForStatementNode *node) {
    printNode(node);
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

void AstTestCasePrinter::visitArrayAccessNode(ArrayAccessNode *node) {}

void printAstTestCase(const Program &program, AstNode *root) {
    if (root == nullptr) {
        std::cerr << "Could not print AST test case (nullptr)." << std::endl;
        return;
    }

    std::cout << std::endl;
    std::cout << "TEST_CASE(\"ASTGenerator can handle '" << program.toString() << "'\") {" << std::endl;
    std::cout << "    std::vector<AstNodeSpec> spec = {" << std::endl;

    auto testCasePrinter = new AstTestCasePrinter();
    root->accept(testCasePrinter);

    std::cout << "    };" << std::endl;
    std::cout << "    std::vector<std::string> program = {\"" << program.toString() << "\"};" << std::endl;
    std::cout << "    assertProgramCreatesAst(program, spec);" << std::endl;
    std::cout << "}" << std::endl;
    std::cout << std::endl;
}
