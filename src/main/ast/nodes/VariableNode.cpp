#include "VariableNode.h"

#include "../AstVisitor.h"

void VariableNode::accept(AstVisitor *v) { v->visitVariableNode(this); }
