#pragma once

#include "AstNode.h"

class VariableDefinitionNode : public AstNode {
  public:
    explicit VariableDefinitionNode(std::string name, ast::DataType type, unsigned int arraySize)
        : AstNode(ast::NodeType::VARIABLE_DEFINITION), name(std::move(name)), type(type), arraySize(arraySize) {}

    void accept(AstVisitor *v) override;

    std::string &getName();

    ast::DataType getType();

    unsigned int getArraySize() { return this->arraySize; }

    bool isArray() { return this->arraySize > 0; }

  private:
    std::string name;
    ast::DataType type;
    unsigned int arraySize;
};
