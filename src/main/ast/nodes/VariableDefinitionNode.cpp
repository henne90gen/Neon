#include "VariableDefinitionNode.h"

#include "../ASTVisitor.h"

void VariableDefinitionNode::accept(ASTVisitor *v) { v->visitVariableDefinitionNode(this); }

std::string &VariableDefinitionNode::getName() { return name; }

ast::DataType VariableDefinitionNode::getType() { return type; }
