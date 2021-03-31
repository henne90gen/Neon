#include "VariableDefinitionNode.h"

#include <utility>

#include <utility>

#include "../AstVisitor.h"

VariableDefinitionNode::VariableDefinitionNode(std::string name, ast::DataType type)
    : AstNode(ast::NodeType::VARIABLE_DEFINITION), name(std::move(name)), type(std::move(std::move(type))), arraySize(0) {}

VariableDefinitionNode::VariableDefinitionNode(std::string name, ast::DataType type, unsigned int arraySize)
    : AstNode(ast::NodeType::VARIABLE_DEFINITION), name(std::move(name)), type(std::move(std::move(type))), arraySize(arraySize) {}

void VariableDefinitionNode::accept(AstVisitor *v) { v->visitVariableDefinitionNode(this); }

std::string &VariableDefinitionNode::getName() { return name; }

ast::DataType VariableDefinitionNode::getType() { return type; }

unsigned int VariableDefinitionNode::getArraySize() const { return this->arraySize; }

bool VariableDefinitionNode::isArray() const { return this->arraySize > 0; }
