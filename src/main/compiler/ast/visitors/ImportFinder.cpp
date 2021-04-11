#include "ImportFinder.h"

void ImportFinder::visitSequenceNode(SequenceNode *node) {
    for (auto *child : node->children) {
        visitNode(child);
    }
}

void ImportFinder::visitStatementNode(StatementNode *node) { visitNode(node->child); }

void ImportFinder::visitImportNode(ImportNode *node) {
    auto path = directoryPath / std::filesystem::path(node->fileName);
    importedModules.push_back(path.string());
}

std::vector<std::string> ImportFinder::run(AST &tree) {
    visitNode(tree.root());
    return importedModules;
}

void ImportFinder::visitNode(AstNode *node) {
    switch (node->type) {
    case ast::NodeType::SEQUENCE:
        visitSequenceNode(&node->sequence);
        break;
    case ast::NodeType::STATEMENT:
        visitStatementNode(&node->statement);
        break;
    case ast::NodeType::IMPORT:
        visitImportNode(&node->import);
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
    case ast::NodeType::TYPE_DECLARATION:
    case ast::NodeType::TYPE_MEMBER:
    case ast::NodeType::MEMBER_ACCESS:
    case ast::NodeType::ASSERT:
    case ast::NodeType::COMMENT:
        break;
    }
}
