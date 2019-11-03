#pragma once

#include "AstNode.h"

class VariableDefinitionNode : public AstNode {
  public:
    explicit VariableDefinitionNode(std::string name, ast::DataType type)
          : AstNode(ast::NodeType::VARIABLE_DEFINITION), name(std::move(name)), type(type) {}

    void accept(AstVisitor *v) override;

    std::string &getName();

    ast::DataType getType();

  private:
    std::string name;
    ast::DataType type;
};
