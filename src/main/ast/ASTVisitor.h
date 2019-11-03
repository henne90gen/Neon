#pragma once

class AssignmentNode;
class BinaryOperationNode;
class BoolNode;
class CallNode;
class FloatNode;
class ForStatementNode;
class FunctionNode;
class IfStatementNode;
class IntegerNode;
class SequenceNode;
class StatementNode;
class UnaryOperationNode;
class VariableDefinitionNode;
class VariableNode;

class ASTVisitor {
  public:
    virtual void visitAssignmentNode(AssignmentNode *node) = 0;
    virtual void visitBinaryOperationNode(BinaryOperationNode *node) = 0;
    virtual void visitBoolNode(BoolNode *node) = 0;
    virtual void visitCallNode(CallNode *node) = 0;
    virtual void visitFloatNode(FloatNode *node) = 0;
    virtual void visitForStatementNode(ForStatementNode *node) = 0;
    virtual void visitFunctionNode(FunctionNode *node) = 0;
    virtual void visitIfStatementNode(IfStatementNode *node) = 0;
    virtual void visitIntegerNode(IntegerNode *node) = 0;
    virtual void visitSequenceNode(SequenceNode *node) = 0;
    virtual void visitStatementNode(StatementNode *node) = 0;
    virtual void visitUnaryOperationNode(UnaryOperationNode *node) = 0;
    virtual void visitVariableDefinitionNode(VariableDefinitionNode *node) = 0;
    virtual void visitVariableNode(VariableNode *node) = 0;
};
