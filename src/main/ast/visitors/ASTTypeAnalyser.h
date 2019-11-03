#pragma once

#include "../ASTVisitor.h"
#include "../Types.h"
#include <unordered_map>

class AstNode;

class ASTTypeAnalyser : public ASTVisitor {
  public:

    void visitAssignmentNode(AssignmentNode *node) override;
    void visitBinaryOperationNode(BinaryOperationNode *node) override;
    void visitBoolNode(BoolNode *node) override;
    void visitCallNode(CallNode *node) override;
    void visitFloatNode(FloatNode *node) override;
    void visitForStatementNode(ForStatementNode *node) override;
    void visitFunctionNode(FunctionNode *node) override;
    void visitIfStatementNode(IfStatementNode *node) override;
    void visitIntegerNode(IntegerNode *node) override;
    void visitSequenceNode(SequenceNode *node) override;
    void visitStatementNode(StatementNode *node) override;
    void visitUnaryOperationNode(UnaryOperationNode *node) override;
    void visitVariableNode(VariableNode *node) override;
    void visitVariableDefinitionNode(VariableDefinitionNode *node) override;


  private:
    std::unordered_map<AstNode *, ast::DataType> typeMap = {};
    std::unordered_map<std::string, ast::DataType> variableMap = {};
    std::unordered_map<std::string, ast::DataType> functionMap = {};
};

void analyseTypes(AstNode *root);
