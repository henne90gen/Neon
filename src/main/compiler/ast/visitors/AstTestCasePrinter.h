#pragma once

#include "../../../Module.h"
#include "../AstNode.h"

class AstTestCasePrinter {
    const Module *module;
    int indentation = 0;

  public:
    explicit AstTestCasePrinter(const Module *module) : module(module) {}

    void run();

  private:
    void printNode(AstNodeID node) const;

    void visitNode(AstNode *node);
    void visitAssignmentNode(AssignmentNode *node);
    void visitBinaryOperationNode(BinaryOperationNode *node);
    void visitCallNode(CallNode *node);
    void visitForStatementNode(ForStatementNode *node);
    void visitFunctionNode(FunctionNode *node);
    void visitIfStatementNode(IfStatementNode *node);
    void visitImportNode(ImportNode *node);
    void visitLiteralNode(LiteralNode *node);
    void visitMemberAccessNode(MemberAccessNode *node);
    void visitSequenceNode(SequenceNode *node);
    void visitStatementNode(StatementNode *node);
    void visitTypeDeclarationNode(TypeDeclarationNode *node);
    void visitTypeMemberNode(TypeMemberNode *node);
    void visitUnaryOperationNode(UnaryOperationNode *node);
    void visitVariableNode(VariableNode *node);
    void visitVariableDefinitionNode(VariableDefinitionNode *node);
};
