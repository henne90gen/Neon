#pragma once

#include <sstream>

#include "../../../Module.h"
#include "../AstNode.h"

class AstPrinter {
    int indentation = 0;
    std::stringstream ss;

  public:
    std::string run(AST *tree);

  private:
    void indent();
    void visitNode(AstNode *node);
    void visitAssertNode(AssertNode *node);
    void visitAssignmentNode(AssignmentNode *node);
    void visitBinaryOperationNode(BinaryOperationNode *node);
    void visitCallNode(CallNode *node);
    void visitForStatementNode(ForStatementNode *node);
    void visitFunctionNode(FunctionNode *node);
    void visitIfStatementNode(IfStatementNode *node);
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
