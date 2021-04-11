#pragma once

#include "Types.h"
#include <string>
#include <vector>

#define AST_NODE(n) reinterpret_cast<AstNode *>(n)

struct AstNode;
typedef int64_t AstNodeID;
enum class LiteralType { BOOL, INTEGER, FLOAT, STRING };

struct AssertNode {
    AstNode *condition = nullptr;
};

struct AssignmentNode {
    AstNode *left = nullptr;
    AstNode *right = nullptr;
};

struct BinaryOperationNode {
    ast::BinaryOperationType type;
    AstNode *left = nullptr;
    AstNode *right = nullptr;

    std::string operation_string();
};

struct CallNode {
    std::string name;
    std::vector<AstNode *> arguments = {};
};

struct CommentNode {
    std::string content;
};

struct ForStatementNode {
    AstNode *init = nullptr;
    AstNode *condition = nullptr;
    AstNode *update = nullptr;
    AstNode *body = nullptr;
};

struct VariableDefinitionNode;
struct FunctionNode {
    std::string name;
    ast::DataType returnType;
    AstNode *body = nullptr;
    std::vector<VariableDefinitionNode *> arguments = {};

    [[nodiscard]] bool is_external() const { return body == nullptr; }
};

struct IfStatementNode {
    AstNode *condition = nullptr;
    AstNode *ifBody = nullptr;
    AstNode *elseBody = nullptr;
};

struct ImportNode {
    std::string fileName;
};

struct LiteralNode {
    LiteralNode() {}

    LiteralType type;
    union {
        bool b;
        int64_t i;
        double d;
        std::string s;
    };
};

struct VariableNode;
struct MemberAccessNode {
    AstNode *left = nullptr;
    AstNode *right = nullptr;

    std::vector<VariableNode *> linearize_access_tree();
};

struct SequenceNode {
    std::vector<AstNode *> children;
};

struct StatementNode {
    AstNode *child = nullptr;
    bool returnStatement;
};

struct TypeMemberNode {
    VariableDefinitionNode *variable_definition = nullptr;
};

struct TypeDeclarationNode {
    std::string name;
    std::vector<TypeMemberNode *> members = {};

    [[nodiscard]] ast::DataType type() const { return ast::DataType(name); }
};

struct UnaryOperationNode {
    ast::UnaryOperationType type;
    AstNode *child = nullptr;
};

struct VariableDefinitionNode {
    std::string name;
    ast::DataType type;
    int64_t arraySize;
    bool is_array() const;
};

struct VariableNode {
    std::string name;
    AstNode *arrayIndex = nullptr;

    [[nodiscard]] bool is_array_access() const;
};

struct AstNode {

    AstNode() {}
    ~AstNode() {}

    union {
        AssertNode assert;
        AssignmentNode assignment;
        BinaryOperationNode binary_operation;
        CallNode call;
        CommentNode comment;
        ForStatementNode for_statement;
        FunctionNode function;
        IfStatementNode if_statement;
        ImportNode import;
        LiteralNode literal;
        MemberAccessNode member_access;
        SequenceNode sequence;
        StatementNode statement;
        TypeDeclarationNode type_declaration;
        TypeMemberNode type_member;
        UnaryOperationNode unary_operation;
        VariableDefinitionNode variable_definition;
        VariableNode variable;
    };

    ast::NodeType type;
};

std::string to_string(ast::NodeType type);
std::string to_string(AstNode *node);
