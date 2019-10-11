#pragma once

#include "AstNode.h"

class VariableNode : public AstNode {
  public:
    explicit VariableNode(std::string name) : AstNode(ast::NodeType::VARIABLE), name(std::move(name)) {}

    void accept(ASTVisitor *v) override;

    std::string &getName() { return name; }

  private:
    std::string name;
};
