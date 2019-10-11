#pragma once

#include "AstNode.h"

#include <vector>

class SequenceNode : public AstNode {
  public:
    SequenceNode() : AstNode(ast::NodeType::SEQUENCE) {}

    void accept(ASTVisitor *v) override;

    std::vector<AstNode *> &getChildren() { return children; }

  private:
    std::vector<AstNode *> children = {};
};
