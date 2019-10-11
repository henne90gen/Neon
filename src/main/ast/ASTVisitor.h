#pragma once

class FunctionNode;
class VariableNode;
class VariableDefinitionNode;
class BinaryOperationNode;
class UnaryOperationNode;
class AssignmentNode;
class SequenceNode;
class StatementNode;
class FloatNode;
class IntegerNode;
class BoolNode;
class CallNode;
class IfStatementNode;

class ASTVisitor {
  public:
    virtual void visitFunctionNode(FunctionNode *node) = 0;
    virtual void visitCallNode(CallNode *node) = 0;
    virtual void visitIfStatementNode(IfStatementNode *node) = 0;
    virtual void visitVariableNode(VariableNode *node) = 0;
    virtual void visitVariableDefinitionNode(VariableDefinitionNode *node) = 0;
    virtual void visitUnaryOperationNode(UnaryOperationNode *node) = 0;
    virtual void visitBinaryOperationNode(BinaryOperationNode *node) = 0;
    virtual void visitAssignmentNode(AssignmentNode *node) = 0;
    virtual void visitSequenceNode(SequenceNode *node) = 0;
    virtual void visitStatementNode(StatementNode *node) = 0;
    virtual void visitFloatNode(FloatNode *node) = 0;
    virtual void visitIntegerNode(IntegerNode *node) = 0;
    virtual void visitBoolNode(BoolNode *node) = 0;
};
