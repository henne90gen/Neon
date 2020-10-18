#pragma once

#include "AstNode.h"

class VariableDefinitionNode : public AstNode {
  public:
    explicit VariableDefinitionNode(std::string name, ast::DataType type);
    explicit VariableDefinitionNode(std::string name, ast::DataType type, unsigned int arraySize);

    void accept(AstVisitor *v) override;

    std::string &getName();

    ast::DataType getType();

    unsigned int getArraySize();

    bool isArray();

  private:
    std::string name;
    ast::DataType type;
    unsigned int arraySize;
};
