#pragma once

#include "AstNode.h"

class AssertNode : public AstNode {
  public:
    explicit AssertNode() : AstNode(ast::NodeType::ASSERT) {}

    void accept(AstVisitor *v) override;

    AstNode *getCondition();
    void setCondition(AstNode *condition);

  private:
    AstNode *condition = nullptr;
};
