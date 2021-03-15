#include "LiteralNodes.h"

#include "../AstVisitor.h"

void BoolNode::accept(AstVisitor *v) { v->visitBoolNode(this); }

void IntegerNode::accept(AstVisitor *v) { v->visitIntegerNode(this); }
std::string IntegerNode::toString() { return std::to_string(value); }

void FloatNode::accept(AstVisitor *v) { v->visitFloatNode(this); }
std::string FloatNode::toString() { return std::to_string(value); }

void StringNode::accept(AstVisitor *v) { v->visitStringNode(this); }
std::string StringNode::toString() { return value; }
