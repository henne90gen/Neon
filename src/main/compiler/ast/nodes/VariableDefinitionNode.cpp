#include "VariableDefinitionNode.h"

#include "../AstVisitor.h"

VariableDefinitionNode::VariableDefinitionNode(std::string name, ast::DataType type, unsigned int arraySize)
    : AstNode(ast::NodeType::VARIABLE_DEFINITION), name(std::move(name)), type(type), arraySize(arraySize) {}

void VariableDefinitionNode::accept(AstVisitor *v) { v->visitVariableDefinitionNode(this); }

std::string &VariableDefinitionNode::getName() { return name; }

ast::DataType VariableDefinitionNode::getType() { return type; }

unsigned int VariableDefinitionNode::getArraySize() { return this->arraySize; }

bool VariableDefinitionNode::isArray() { return this->arraySize > 0; }
