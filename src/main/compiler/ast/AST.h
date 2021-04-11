#pragma once

#include "AstNode.h"

class AST {
    AstNode *buffer = nullptr;
    int64_t nextNodeID = 1;
    uint64_t nodeCapacity = 1000000;
    bool complete = false;

  public:
    AST();

    AstNode *root();
    void completed();

    StatementNode *createStatement(AstNode *child, bool isReturn);
    AssertNode *createAssert(AstNode *condition);
    AssignmentNode *createAssignment(AstNode *left, AstNode *right);
    BinaryOperationNode *createBinaryOperation(ast::BinaryOperationType type, AstNode *left, AstNode *right);
    UnaryOperationNode *createUnaryOperation(ast::UnaryOperationType type, AstNode *child);
    CallNode *createCall(std::string name, std::vector<AstNode *> parameters);
    FunctionNode *createFunction(std::string name, ast::DataType returnType,
                                 std::vector<VariableDefinitionNode *> parameters, SequenceNode *body);
    IfStatementNode *createIf(AstNode *condition, SequenceNode *ifBody, SequenceNode *elseBody);
    ForStatementNode *createFor(StatementNode *init, AstNode *condition, StatementNode *update, SequenceNode *body);
    CommentNode *createComment(std::string content);
    VariableNode *createVariable(std::string name, AstNode *arrayIndex);
    ImportNode *createImport(std::string fileName);
    VariableDefinitionNode *createVariableDefinition(std::string name, ast::DataType type, int64_t arraySize);
    SequenceNode *createSequence(std::vector<AstNode *> children);
    MemberAccessNode *createMemberAccess(AstNode *left, AstNode *right);

    LiteralNode *createLiteralInteger(int64_t value);

    LiteralNode *createLiteralFloat(double value);

    LiteralNode *createLiteralBool(bool value);

    LiteralNode *createLiteralString(std::string value);

    TypeMemberNode *createTypeMember(VariableDefinitionNode *inner);

    TypeDeclarationNode *createTypeDeclaration(std::string name, std::vector<TypeMemberNode *> members);

    bool is_complete() const;

  private:
    template <typename T> T *createNode(ast::NodeType type);
};
