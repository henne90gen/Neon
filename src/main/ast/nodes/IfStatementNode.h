#pragma once

class IfStatementNode : public AstNode {
  public:
    explicit IfStatementNode() : AstNode(AstNode::IF_STATEMENT) {}

    void accept(ASTVisitor *v) override { v->visitIfStatementNode(this); }

    AstNode *getCondition() { return condition; }
    void setCondition(AstNode *condition) { this->condition = condition; }

    AstNode *getIfBody() { return ifBody; }
    void setIfBody(AstNode *ifBody) { this->ifBody = ifBody; }

    AstNode *getElseBody() { return elseBody; }
    void setElseBody(AstNode *elseBody) { this->elseBody = elseBody; }

  private:
    AstNode *condition = nullptr;
    AstNode *ifBody = nullptr;
    AstNode *elseBody = nullptr;
};
