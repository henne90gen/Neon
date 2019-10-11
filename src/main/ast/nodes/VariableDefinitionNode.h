#pragma once

class VariableDefinitionNode : public AstNode {
  public:
    explicit VariableDefinitionNode(std::string name, DataType type)
          : AstNode(AstNode::VARIABLE_DEFINITION), name(std::move(name)), type(type) {}

    void accept(ASTVisitor *v) override { v->visitVariableDefinitionNode(this); };

    std::string &getName() { return name; }

    DataType getType() { return type; }

  private:
    std::string name;
    DataType type;
};
