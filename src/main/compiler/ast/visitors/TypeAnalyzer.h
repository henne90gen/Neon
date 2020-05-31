#pragma once

#include "../../FunctionResolver.h"
#include "../AstVisitor.h"
#include "../Types.h"
#include <unordered_map>

class TypeAnalyzer : public AstVisitor {
  public:
    explicit TypeAnalyzer(Module *module, const FunctionResolver &functionResolver)
        : module(module), functionResolver(functionResolver) {}

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
    void visitStringNode(StringNode *node) override;
    void visitTypeDeclarationNode(TypeDeclarationNode *node) override;
    void visitUnaryOperationNode(UnaryOperationNode *node) override;
    void visitVariableNode(VariableNode *node) override;
    void visitVariableDefinitionNode(VariableDefinitionNode *node) override;

    std::pair<std::unordered_map<AstNode *, ast::DataType>, std::unordered_map<std::string, ast::DataType>>
    run(AstNode *root);

  private:
    Module *module;
    const FunctionResolver &functionResolver;

    std::unordered_map<AstNode *, ast::DataType> nodeTypeMap = {};
    std::unordered_map<std::string, ast::DataType> variableTypeMap = {};
    std::unordered_map<ast::DataType, ComplexType> complexTypeMap = {};
};
