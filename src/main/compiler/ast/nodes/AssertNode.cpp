#include "AssertNode.h"

#include "../AstVisitor.h"

void AssertNode::accept(AstVisitor *v) { v->visitAssertNode(this); }

AstNode *AssertNode::getCondition() { return condition; }

void AssertNode::setCondition(AstNode *condition) { this->condition = condition; }
