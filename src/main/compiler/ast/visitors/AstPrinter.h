#pragma once

#include <sstream>

#include "../../../Module.h"
#include "../AstVisitor.h"
#include "../nodes/AstNode.h"

class AstPrinter : public AstVisitor {
  public:
    explicit AstPrinter(const Module *module) : module(module) {}

    void visitAssertNode(AssertNode *node) override;
    void visitAssignmentNode(AssignmentNode *node) override;
    void visitBinaryOperationNode(BinaryOperationNode *node) override;
    void visitBoolNode(BoolNode *node) override;
    void visitCallNode(CallNode *node) override;
    void visitFloatNode(FloatNode *node) override;
    void visitForStatementNode(ForStatementNode *node) override;
    void visitFunctionNode(FunctionNode *node) override;
    void visitIfStatementNode(IfStatementNode *node) override;
    void visitIntegerNode(IntegerNode *node) override;
    void visitMemberAccessNode(MemberAccessNode *node) override;
    void visitSequenceNode(SequenceNode *node) override;
    void visitStatementNode(StatementNode *node) override;
    void visitTypeDeclarationNode(TypeDeclarationNode *node) override;
    void visitTypeMemberNode(TypeMemberNode *node) override;
    void visitUnaryOperationNode(UnaryOperationNode *node) override;
    void visitVariableNode(VariableNode *node) override;
    void visitVariableDefinitionNode(VariableDefinitionNode *node) override;

    std::string run();

  private:
    const Module *module;
    int indentation = 0;
    std::stringstream ss;

    void indent();
};
