#include "VariableNode.h"

#include "../ASTVisitor.h"

void VariableNode::accept(ASTVisitor *v) { v->visitVariableNode(this); }
