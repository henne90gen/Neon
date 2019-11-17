#include "ArrayAccessNode.h"

#include "../AstVisitor.h"

void ArrayAccessNode::accept(AstVisitor *v) { v->visitArrayAccessNode(this); }
