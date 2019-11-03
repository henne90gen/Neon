#pragma once

#include "AstNode.h"

class IfStatementNode : public AstNode {
  public:
    explicit IfStatementNode() : AstNode(ast::NodeType::IF_STATEMENT) {}

    void accept(AstVisitor *v) override;

    AstNode *getCondition();
    void setCondition(AstNode *condition);

    AstNode *getIfBody();
    void setIfBody(AstNode *ifBody);

    AstNode *getElseBody();
    void setElseBody(AstNode *elseBody);

  private:
    AstNode *condition = nullptr;
    AstNode *ifBody = nullptr;
    AstNode *elseBody = nullptr;
};
