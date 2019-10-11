#pragma once

#include "../ASTVisitor.h"
#include "../nodes/AstNode.h"

class ASTPrinter : public ASTVisitor {
  public:
    explicit ASTPrinter() = default;

    void visitFunctionNode(FunctionNode *node) override;
    void visitCallNode(CallNode *node) override;
    void visitIfStatementNode(IfStatementNode *node) override;
    void visitVariableNode(VariableNode *node) override;
    void visitVariableDefinitionNode(VariableDefinitionNode *node) override;
    void visitBinaryOperationNode(BinaryOperationNode *node) override;
    void visitUnaryOperationNode(UnaryOperationNode *node) override;
    void visitAssignmentNode(AssignmentNode *node) override;
    void visitSequenceNode(SequenceNode *node) override;
    void visitStatementNode(StatementNode *node) override;
    void visitFloatNode(FloatNode *node) override;
    void visitIntegerNode(IntegerNode *node) override;
    void visitBoolNode(BoolNode *node) override;

  private:
    int indentation = 0;
};

void printAst(AstNode *root);
