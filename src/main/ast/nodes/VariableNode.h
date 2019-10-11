#pragma once

class VariableNode : public AstNode {
  public:
    explicit VariableNode(std::string name) : AstNode(AstNode::VARIABLE), name(std::move(name)) {}

    void accept(ASTVisitor *v) override { v->visitVariableNode(this); };

    std::string &getName() { return name; }

  private:
    std::string name;
};
