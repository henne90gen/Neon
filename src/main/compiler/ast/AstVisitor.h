#pragma once

class AssignmentNode;
class BinaryOperationNode;
class BoolNode;
class CallNode;
class FloatNode;
class ForStatementNode;
class FunctionNode;
class IfStatementNode;
class ImportNode;
class IntegerNode;
class SequenceNode;
class StatementNode;
class StringNode;
class UnaryOperationNode;
class VariableDefinitionNode;
class VariableNode;

class AstVisitor {
  public:
    virtual void visitAssignmentNode(AssignmentNode *node){};
    virtual void visitBinaryOperationNode(BinaryOperationNode *node){};
    virtual void visitBoolNode(BoolNode *node){};
    virtual void visitCallNode(CallNode *node){};
    virtual void visitFloatNode(FloatNode *node){};
    virtual void visitForStatementNode(ForStatementNode *node){};
    virtual void visitFunctionNode(FunctionNode *node){};
    virtual void visitIfStatementNode(IfStatementNode *node){};
    virtual void visitImportNode(ImportNode *node){};
    virtual void visitIntegerNode(IntegerNode *node){};
    virtual void visitSequenceNode(SequenceNode *node){};
    virtual void visitStatementNode(StatementNode *node){};
    virtual void visitStringNode(StringNode *node){};
    virtual void visitUnaryOperationNode(UnaryOperationNode *node){};
    virtual void visitVariableDefinitionNode(VariableDefinitionNode *node){};
    virtual void visitVariableNode(VariableNode *node){};
};
