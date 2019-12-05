#pragma once

#include "AstNode.h"

class ForStatementNode : public AstNode {
  public:
    explicit ForStatementNode() : AstNode(ast::NodeType::FOR_STATEMENT) {}

    void accept(AstVisitor *v) override;

    AstNode *getInit() { return init; }
    void setInit(AstNode *init) { this->init = init; }

    AstNode *getCondition() { return condition; }
    void setCondition(AstNode *condition) { this->condition = condition; }

    AstNode *getUpdate() { return update; }
    void setUpdate(AstNode *update) { this->update = update; }

    AstNode *getBody() { return body; }
    void setBody(AstNode *body) { this->body = body; }

  private:
    AstNode *init = nullptr;
    AstNode *condition = nullptr;
    AstNode *update = nullptr;
    AstNode *body = nullptr;
};
