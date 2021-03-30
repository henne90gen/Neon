#include "AstPrinter.h"

#include <iostream>

#include "../nodes/AllNodes.h"
#include "util/Utils.h"

void AstPrinter::indent() {
    for (int i = 0; i < indentation; i++) {
        ss << "  ";
    }
}

std::string printUnaryOperationType(ast::UnaryOperationType type) {
    if (type == ast::UnaryOperationType::NOT) {
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
    indent();
    ss << "FunctionNode(name='" << node->getName() << "', numArguments=" << node->getArguments().size() << ")"
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
    indent();
    ss << "VariableNode(name='" << node->getName() << "', isArrayAccess='" << node->isArrayAccess() << "', arrayIndex='"
       << node->getArrayIndex() << "')" << std::endl;
}

void AstPrinter::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    indent();
    ss << "VariableDefinitionNode(type='" << to_string(node->getType()) << "', name='" << node->getName()
       << "', isArray='" << node->isArray() << "', arraySize='" << node->getArraySize() << "')" << std::endl;
}

void AstPrinter::visitBinaryOperationNode(BinaryOperationNode *node) {
    indent();
    std::string operationType = printBinaryOperationType(node->getType());
    ss << "BinaryOperationNode(hasLeft=" << (node->getLeft() != nullptr)
       << ", hasRight=" << (node->getRight() != nullptr) << ", type=" << operationType << ")" << std::endl;
    indentation++;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indentation--;
}

void AstPrinter::visitUnaryOperationNode(UnaryOperationNode *node) {
    indent();
    std::string operationType = printUnaryOperationType(node->getType());
    ss << "UnaryOperationNode(hasChild=" << (node->getChild() != nullptr) << ", type=" << operationType << ")"
       << std::endl;
    indentation++;
    node->getChild()->accept(this);
    indentation--;
}

void AstPrinter::visitSequenceNode(SequenceNode *node) {
    indent();
    ss << "SequenceNode(size=" << node->getChildren().size() << ")" << std::endl;
    indentation++;
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
    indentation--;
}

void AstPrinter::visitStatementNode(StatementNode *node) {
    indent();
    ss << "StatementNode(isReturnStatement=" << node->isReturnStatement() << ")" << std::endl;
    indentation++;
    if (node->getChild() != nullptr) {
        node->getChild()->accept(this);
    }
    indentation--;
}

void AstPrinter::visitFloatNode(FloatNode *node) {
    indent();
    ss << "FloatNode(value=" << node->getValue() << ")" << std::endl;
}

void AstPrinter::visitIntegerNode(IntegerNode *node) {
    indent();
    ss << "IntegerNode(value=" << node->getValue() << ")" << std::endl;
}

void AstPrinter::visitBoolNode(BoolNode *node) {
    indent();
    ss << "BoolNode(value=" << node->getValue() << ")" << std::endl;
}

void AstPrinter::visitAssignmentNode(AssignmentNode *node) {
    indent();
    ss << "AssignmentNode(hasLeft=" << (node->getLeft() != nullptr) << ", hasRight=" << (node->getRight() != nullptr)
       << ")" << std::endl;
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
    indent();
    ss << "CallNode(name='" << node->getName() << "', numArguments=" << node->getArguments().size() << ")" << std::endl;
    indentation++;
    for (auto argument : node->getArguments()) {
        argument->accept(this);
    }
    indentation--;
}

void AstPrinter::visitIfStatementNode(IfStatementNode *node) {
    indent();
    ss << "IfStatementNode()" << std::endl;
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
    indent();
    ss << "ForStatementNode()" << std::endl;
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

void AstPrinter::visitTypeDeclarationNode(TypeDeclarationNode *node) {
    indent();
    ss << "TypeDeclarationNode(name='" << node->getName() << "')" << std::endl;
    indentation++;
    for (const auto member : node->getMembers()) {
        member->accept(this);
    }
    indentation--;
}

void AstPrinter::visitTypeMemberNode(TypeMemberNode *node) {
    indent();
    ss << "TypeMemberNode(name='" << node->getName() << "', type='" << to_string(node->getType()) << "')" << std::endl;
}

void AstPrinter::visitMemberAccessNode(MemberAccessNode *node) {
    indent();
    // TODO print all chained member accesses
    ss << "MemberAccessNode" << std::endl;
    indentation++;
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    indentation--;
}

void AstPrinter::visitAssertNode(AssertNode *node) {
    indent();
    ss << "AssertNode()" << std::endl;
    indentation++;
    node->getCondition()->accept(this);
    indentation--;
}

std::string AstPrinter::run() {
    if (module->root == nullptr) {
        return "Could not print AST (nullptr).";
    }

    module->root->accept(this);
    return ss.str();
}
