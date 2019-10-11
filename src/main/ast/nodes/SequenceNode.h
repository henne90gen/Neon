#pragma once

class SequenceNode : public AstNode {
  public:
    SequenceNode() : AstNode(AstNode::SEQUENCE) {}

    void accept(ASTVisitor *v) override { v->visitSequenceNode(this); };

    std::vector<AstNode *> &getChildren() { return children; }

  private:
    std::vector<AstNode *> children = {};
};
