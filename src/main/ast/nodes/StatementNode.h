#pragma once

class StatementNode : public AstNode {
  public:
    StatementNode() : AstNode(AstNode::STATEMENT) {}

    void accept(ASTVisitor *v) override { v->visitStatementNode(this); };

    bool isReturnStatement() { return this->returnStatement; }
    void setIsReturnStatement(bool returnStatement) { this->returnStatement = returnStatement; }

    AstNode *getChild() { return child; }
    void setChild(AstNode *child) { this->child = child; }

  private:
    bool returnStatement = false;
    AstNode *child = nullptr;
};
