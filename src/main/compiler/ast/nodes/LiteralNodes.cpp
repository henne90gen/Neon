#include "LiteralNodes.h"

#include "../AstVisitor.h"

void BoolNode::accept(AstVisitor *v) { v->visitBoolNode(this); }

void IntegerNode::accept(AstVisitor *v) { v->visitIntegerNode(this); }

void FloatNode::accept(AstVisitor *v) { v->visitFloatNode(this); }

void StringNode::accept(AstVisitor *v) { v->visitStringNode(this); }
