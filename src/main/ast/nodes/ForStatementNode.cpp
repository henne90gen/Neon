#include "ForStatementNode.h"

#include "../AstVisitor.h"

void ForStatementNode::accept(AstVisitor *v) { v->visitForStatementNode(this); }
