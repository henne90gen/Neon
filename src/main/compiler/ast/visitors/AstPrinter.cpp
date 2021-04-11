#include "AstPrinter.h"

#include "util/Utils.h"

// TODO enable this again
#if 0

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
    ss << "FunctionNode(name='" << node->name << "', numArguments=" << node->arguments.size() << ")" << std::endl;
    indentation++;
    for (auto &argument : node->arguments) {
        visitNode(argument);
    }
    if (!node->is_external()) {
        visitNode(node->body);
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
    for (auto *child : node->getChildren()) {
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
    for (auto *argument : node->getArguments()) {
        argument->accept(this);
    }
    indentation--;
}

void AstPrinter::visitIfStatementNode(AstNode *node) {
    indent();
    ss << "IfStatementNode()" << std::endl;
    indentation++;
    visitNode(node->if_statement.condition);
    if (node->if_statement.ifBody != nullptr) {
        visitNode(node->if_statement.ifBody);
    }
    if (node->if_statement.elseBody != nullptr) {
        visitNode(node->if_statement.elseBody);
    }
    indentation--;
}

void AstPrinter::visitForStatementNode(AstNode *node) {
    indent();
    ss << "ForStatementNode()" << std::endl;
    indentation++;
    if (node->for_statement.init != nullptr) {
        visitNode(node->for_statement.init);
    }
    if (node->for_statement.condition != nullptr) {
        visitNode(node->for_statement.condition);
    }
    if (node->for_statement.update != nullptr) {
        visitNode(node->for_statement.update);
    }
    if (node->for_statement.body != nullptr) {
        visitNode(node->for_statement.body);
    }
    indentation--;
}

void AstPrinter::visitTypeDeclarationNode(AstNode *node) {
    indent();
    ss << "TypeDeclarationNode(name='" << node->type_declaration.name << "')" << std::endl;
    indentation++;
    for (auto member : node->type_declaration.members) {
        visitNode(member);
    }
    indentation--;
}

void AstPrinter::visitTypeMemberNode(AstNode *node) {
    indent();
    AstNode *inner = node->type_member.variable_definition;
    ss << "TypeMemberNode(name='" << inner->variable_definition.name << "', type='"
       << to_string(inner->variable_definition.type) << "')" << std::endl;
}

void AstPrinter::visitMemberAccessNode(AstNode *node) {
    indent();
    ss << "MemberAccessNode" << std::endl;
    indentation++;
    visitNode(node->member_access.left);
    visitNode(node->member_access.right);
    indentation--;
}

void AstPrinter::visitAssertNode(AstNode *node) {
    indent();
    ss << "AssertNode()" << std::endl;
    indentation++;
    visitNode(node->assert.condition);
    indentation--;
}

std::string AstPrinter::run(AST *tree) {
    visitNode(tree->root());
    return ss.str();
}

#endif
