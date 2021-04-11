#include "AST.h"

#include <cassert>
#include <iostream>
#include <utility>

AST::AST() {
    size_t size = nodeCapacity * sizeof(AstNode);
    buffer = (AstNode *)std::malloc(size);
}

AstNode *AST::root() {
    // NOTE: the first node in the buffer is assumed to be the root node
    return buffer;
}

template <typename T> T *AST::createNode(ast::NodeType type) {
    assert(nextNodeID < nodeCapacity);
    if (nextNodeID >= nodeCapacity) {
        std::cerr << "Ran out of space for ast nodes!" << std::endl;
    }

    auto node = buffer + nextNodeID;
    nextNodeID++;
    node->type = type;

    auto *result = reinterpret_cast<T *>(node);
    return new (result) T;
}

AssertNode *AST::createAssert(AstNode *condition) {
    auto node = createNode<AssertNode>(ast::NodeType::ASSERT);
    node->condition = condition;
    return node;
}

AssignmentNode *AST::createAssignment(AstNode *left, AstNode *right) {
    auto node = createNode<AssignmentNode>(ast::NodeType::ASSIGNMENT);
    node->left = left;
    node->right = right;
    return node;
}

BinaryOperationNode *AST::createBinaryOperation(ast::BinaryOperationType type, AstNode *left, AstNode *right) {
    auto node = createNode<BinaryOperationNode>(ast::NodeType::BINARY_OPERATION);
    node->type = type;
    node->left = left;
    node->right = right;
    return node;
}

StatementNode *AST::createStatement(AstNode *child, bool isReturn) {
    auto node = createNode<StatementNode>(ast::NodeType::STATEMENT);
    node->child = child;
    node->returnStatement = isReturn;
    return node;
}

UnaryOperationNode *AST::createUnaryOperation(ast::UnaryOperationType type, AstNode *child) {
    auto node = createNode<UnaryOperationNode>(ast::NodeType::UNARY_OPERATION);
    node->type = type;
    node->child = child;
    return node;
}

CallNode *AST::createCall(std::string name, std::vector<AstNode *> parameters) {
    auto node = createNode<CallNode>(ast::NodeType::CALL);
    node->name = std::move(name);
    node->arguments = std::move(parameters);
    return node;
}

FunctionNode *AST::createFunction(std::string name, ast::DataType returnType,
                                  std::vector<VariableDefinitionNode *> parameters, SequenceNode *body) {
    auto node = createNode<FunctionNode>(ast::NodeType::FUNCTION);
    node->name = std::move(name);
    node->returnType = std::move(returnType);
    node->arguments = std::move(parameters);
    node->body = AST_NODE(body);
    return node;
}

IfStatementNode *AST::createIf(AstNode *condition, SequenceNode *ifBody, SequenceNode *elseBody) {
    auto node = createNode<IfStatementNode>(ast::NodeType::IF_STATEMENT);
    node->condition = condition;
    node->ifBody = AST_NODE(ifBody);
    node->elseBody = AST_NODE(elseBody);
    return node;
}

ForStatementNode *AST::createFor(StatementNode *init, AstNode *condition, StatementNode *update, SequenceNode *body) {
    auto node = createNode<ForStatementNode>(ast::NodeType::FOR_STATEMENT);
    node->init = AST_NODE(init);
    node->condition = condition;
    node->update = AST_NODE(update);
    node->body = AST_NODE(body);
    return node;
}

CommentNode *AST::createComment(std::string content) {
    auto node = createNode<CommentNode>(ast::NodeType::COMMENT);
    node->content = std::move(content);
    return node;
}

VariableNode *AST::createVariable(std::string name, AstNode *arrayIndex) {
    auto node = createNode<VariableNode>(ast::NodeType::VARIABLE);
    node->name = std::move(name);
    node->arrayIndex = arrayIndex;
    return node;
}

ImportNode *AST::createImport(std::string fileName) {
    auto node = createNode<ImportNode>(ast::NodeType::IMPORT);
    node->fileName = std::move(fileName);
    return node;
}

VariableDefinitionNode *AST::createVariableDefinition(std::string name, ast::DataType type, int64_t arraySize) {
    auto node = createNode<VariableDefinitionNode>(ast::NodeType::VARIABLE_DEFINITION);
    node->name = std::move(name);
    node->type = std::move(type);
    node->arraySize = arraySize;
    return node;
}

SequenceNode *AST::createSequence(std::vector<AstNode *> children) {
    auto node = createNode<SequenceNode>(ast::NodeType::SEQUENCE);
    node->children = std::move(children);
    return node;
}

MemberAccessNode *AST::createMemberAccess(AstNode *left, AstNode *right) {
    auto node = createNode<MemberAccessNode>(ast::NodeType::MEMBER_ACCESS);
    node->left = left;
    node->right = right;
    return node;
}

LiteralNode *AST::createLiteralFloat(double value) {
    auto node = createNode<LiteralNode>(ast::NodeType::LITERAL);
    node->type = LiteralType::FLOAT;
    node->d = value;
    return node;
}

LiteralNode *AST::createLiteralInteger(int64_t value) {
    auto node = createNode<LiteralNode>(ast::NodeType::LITERAL);
    node->type = LiteralType::INTEGER;
    node->i = value;
    return node;
}

LiteralNode *AST::createLiteralString(std::string value) {
    auto node = createNode<LiteralNode>(ast::NodeType::LITERAL);
    node->type = LiteralType::STRING;
    // NOTE we have to do this, in order for std::string to be initialized correctly
    new (&node->s) std::string;
    node->s = std::move(value);
    return node;
}

LiteralNode *AST::createLiteralBool(bool value) {
    auto node = createNode<LiteralNode>(ast::NodeType::LITERAL);
    node->type = LiteralType::BOOL;
    node->b = value;
    return node;
}

TypeMemberNode *AST::createTypeMember(VariableDefinitionNode *inner) {
    auto node = createNode<TypeMemberNode>(ast::NodeType::TYPE_MEMBER);
    node->variable_definition = inner;
    return node;
}

TypeDeclarationNode *AST::createTypeDeclaration(std::string name, std::vector<TypeMemberNode *> members) {
    auto node = createNode<TypeDeclarationNode>(ast::NodeType::TYPE_DECLARATION);
    node->name = std::move(name);
    node->members = std::move(members);
    return node;
}

void AST::completed() { complete = true; }
bool AST::is_complete() const { return complete; }
