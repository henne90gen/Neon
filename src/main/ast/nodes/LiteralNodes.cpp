#include "LiteralNodes.h"

#include "../ASTVisitor.h"

void IntegerNode::accept(ASTVisitor *v) { v->visitIntegerNode(this); }

void FloatNode::accept(ASTVisitor *v) { v->visitFloatNode(this); }

void BoolNode::accept(ASTVisitor *v) { v->visitBoolNode(this); }
