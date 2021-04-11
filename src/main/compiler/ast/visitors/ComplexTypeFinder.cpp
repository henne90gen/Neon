#include "ComplexTypeFinder.h"
#include "cassert"

std::vector<ComplexType> ComplexTypeFinder::run(AST &tree) {
    visitNode(tree.root());
    return types;
}

void ComplexTypeFinder::visitSequenceNode(AstNode *node) {
    assert(node->type == ast::NodeType::SEQUENCE);
    for (auto child : node->sequence.children) {
        visitNode(child);
    }
}

void ComplexTypeFinder::visitStatementNode(AstNode *node) {
    assert(node->type == ast::NodeType::STATEMENT);
    visitNode(node->statement.child);
}

void ComplexTypeFinder::visitTypeDeclarationNode(AstNode *node) {
    assert(node->type == ast::NodeType::TYPE_DECLARATION);
    ComplexType t = {.type = ast::DataType(node->type_declaration.type())};

    for (auto member : node->type_declaration.members) {
        auto memberNode = member;
        auto variableDefinition = memberNode->variable_definition;
        ComplexTypeMember m = {
              .name = variableDefinition->name,
              .type = variableDefinition->type,
        };
        t.members.push_back(m);
    }

    types.push_back(t);
}

void ComplexTypeFinder::visitNode(AstNode *node) {
    switch (node->type) {
    case ast::NodeType::SEQUENCE:
        visitSequenceNode(node);
        break;
    case ast::NodeType::STATEMENT:
        visitStatementNode(node);
        break;
    case ast::NodeType::TYPE_DECLARATION:
        visitTypeDeclarationNode(node);
        break;
    case ast::NodeType::LITERAL:
    case ast::NodeType::UNARY_OPERATION:
    case ast::NodeType::BINARY_OPERATION:
    case ast::NodeType::FUNCTION:
    case ast::NodeType::CALL:
    case ast::NodeType::VARIABLE_DEFINITION:
    case ast::NodeType::VARIABLE:
    case ast::NodeType::ASSIGNMENT:
    case ast::NodeType::IF_STATEMENT:
    case ast::NodeType::FOR_STATEMENT:
    case ast::NodeType::IMPORT:
    case ast::NodeType::TYPE_MEMBER:
    case ast::NodeType::MEMBER_ACCESS:
    case ast::NodeType::ASSERT:
    case ast::NodeType::COMMENT:
        // do nothing
        break;
    }
}
