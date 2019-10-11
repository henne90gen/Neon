#pragma once

#include "../AST.h"

#include <unordered_map>

class ASTTypeAnalyser : public ASTVisitor {
  public:
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
    std::unordered_map<AstNode *, AstNode::DataType> typeMap = {};
    std::unordered_map<std::string, AstNode::DataType> variableMap = {};
    std::unordered_map<std::string, AstNode::DataType> functionMap = {};
};

void analyseTypes(AstNode *root);
