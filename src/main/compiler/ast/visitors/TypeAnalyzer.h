#pragma once

#include "../../FunctionResolver.h"
#include "../Types.h"
#include <unordered_map>

class TypeAnalyzer {
    const Logger &log;
    Module *module;
    const FunctionResolver &functionResolver;

    std::unordered_map<AstNode *, ast::DataType> nodeTypeMap = {};
    std::unordered_map<std::string, ast::DataType> variableTypeMap = {};
    std::unordered_map<ast::DataType, ComplexType> complexTypeMap = {};

  public:
    explicit TypeAnalyzer(const Logger &log, Module *module, const FunctionResolver &functionResolver)
        : log(log), module(module), functionResolver(functionResolver) {}

    std::pair<std::unordered_map<AstNode *, ast::DataType>, std::unordered_map<std::string, ast::DataType>>
    run(AST &tree);

  private:
    void visitNode(AstNode *node);
    void visitAssertNode(AssertNode *node);
    void visitAssignmentNode(AssignmentNode *node);
    void visitBinaryOperationNode(BinaryOperationNode *node);
    void visitCallNode(CallNode *node);
    void visitForStatementNode(ForStatementNode *node);
    void visitFunctionNode(FunctionNode *node);
    void visitIfStatementNode(IfStatementNode *node);
    void visitMemberAccessNode(MemberAccessNode *node);
    void visitSequenceNode(SequenceNode *node);
    void visitStatementNode(StatementNode *node);
    void visitLiteralNode(LiteralNode *node);
    void visitTypeDeclarationNode(TypeDeclarationNode *node);
    void visitUnaryOperationNode(UnaryOperationNode *node);
    void visitVariableNode(VariableNode *node);
    void visitVariableDefinitionNode(VariableDefinitionNode *node);
};
