#pragma once

#include <unordered_map>

#include "../ASTVisitor.h"
#include "../nodes/AstNode.h"

struct CalculationResult {
    enum CalculationType { INTEGER, FLOAT, BOOL };
    CalculationType type = INTEGER;
    union {
        int intResult;
        float floatResult;
        bool boolResult;
    };
};

class ASTInterpreter : public ASTVisitor {
  public:
    explicit ASTInterpreter(const bool verbose) : verbose(verbose) {}

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
    const bool verbose;
    std::unordered_map<AstNode *, CalculationResult> calculationResults = {};
    std::unordered_map<std::string, FunctionNode *> functions = {};
    std::unordered_map<std::string, VariableDefinitionNode *> variables = {};

    void printStatementResult(AstNode *child);
};

void interpretAst(AstNode *node, bool verbose);
