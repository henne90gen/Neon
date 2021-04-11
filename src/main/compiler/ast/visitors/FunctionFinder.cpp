#include "FunctionFinder.h"

void FunctionFinder::visitSequenceNode(SequenceNode *node) {
    for (auto *child : node->children) {
        visitNode(child);
    }
}

void FunctionFinder::visitStatementNode(StatementNode *node) { visitNode(node->child); }

void FunctionFinder::visitFunctionNode(FunctionNode *node) {
    FunctionSignature funcSig = {
          .name = node->name,
          .returnType = node->returnType,
    };
    for (auto &argument : node->arguments) {
        FunctionArgument funcArg = {
              .name = argument->name,
              .type = argument->type,
        };
        funcSig.arguments.push_back(funcArg);
    }
    functions.push_back(funcSig);
}

void FunctionFinder::visitTypeDeclarationNode(TypeDeclarationNode *node) {
    FunctionSignature funcSig = {
          .name = node->name,
          .returnType = node->type(),
    };
    // TODO(henne): add constructor arguments, maybe...
    functions.push_back(funcSig);
}

std::vector<FunctionSignature> FunctionFinder::run(AST &tree) {
    visitNode(tree.root());
    return functions;
}

void FunctionFinder::visitNode(AstNode *node) {
    switch (node->type) {
    case ast::NodeType::SEQUENCE:
        visitSequenceNode(&node->sequence);
        break;
    case ast::NodeType::STATEMENT:
        visitStatementNode(&node->statement);
        break;
    case ast::NodeType::FUNCTION:
        visitFunctionNode(&node->function);
        break;
    case ast::NodeType::TYPE_DECLARATION:
        visitTypeDeclarationNode(&node->type_declaration);
        break;
    case ast::NodeType::LITERAL:
    case ast::NodeType::UNARY_OPERATION:
    case ast::NodeType::BINARY_OPERATION:
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
