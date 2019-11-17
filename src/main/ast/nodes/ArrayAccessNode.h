#pragma once

#include "AstNode.h"

class ArrayAccessNode : public AstNode {
  public:
    explicit ArrayAccessNode() : AstNode(ast::NodeType::ARRAY_ACCESS) {}

    void accept(AstVisitor *v) override;

  private:
    // array type?
    // array size
    // variableNode
};
