#include "LiteralNodes.h"

#include "../AstVisitor.h"

void IntegerNode::accept(AstVisitor *v) { v->visitIntegerNode(this); }

void FloatNode::accept(AstVisitor *v) { v->visitFloatNode(this); }

void BoolNode::accept(AstVisitor *v) { v->visitBoolNode(this); }
