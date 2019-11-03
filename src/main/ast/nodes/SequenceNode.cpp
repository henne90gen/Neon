#include "SequenceNode.h"

#include "../AstVisitor.h"

void SequenceNode::accept(AstVisitor *v) { v->visitSequenceNode(this); }
