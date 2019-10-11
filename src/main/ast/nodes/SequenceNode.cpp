#include "SequenceNode.h"

#include "../ASTVisitor.h"

void SequenceNode::accept(ASTVisitor *v) { v->visitSequenceNode(this); }
