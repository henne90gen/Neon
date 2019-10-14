#include "VariableDefinitionNode.h"

#include "../ASTVisitor.h"

void VariableDefinitionNode::accept(ASTVisitor *v) { v->visitVariableDefinitionNode(this); }

auto VariableDefinitionNode::getName() -> std::string & { return name; }

auto VariableDefinitionNode::getType() -> ast::DataType { return type; }
