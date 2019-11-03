#include "ForStatementNode.h"

#include "../ASTVisitor.h"

void ForStatementNode::accept(ASTVisitor *v) { v->visitForStatementNode(this); }
