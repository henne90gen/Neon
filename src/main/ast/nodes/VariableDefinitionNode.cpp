#include "VariableDefinitionNode.h"

#include "../AstVisitor.h"

void VariableDefinitionNode::accept(AstVisitor *v) { v->visitVariableDefinitionNode(this); }

std::string &VariableDefinitionNode::getName() { return name; }

ast::DataType VariableDefinitionNode::getType() { return type; }
