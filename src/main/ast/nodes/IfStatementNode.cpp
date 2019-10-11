#include "IfStatementNode.h"

#include "../ASTVisitor.h"

void IfStatementNode::accept(ASTVisitor *v) { v->visitIfStatementNode(this); }

AstNode *IfStatementNode::getCondition() { return condition; }

void IfStatementNode::setCondition(AstNode *condition) { this->condition = condition; }

AstNode *IfStatementNode::getIfBody() { return ifBody; }

void IfStatementNode::setIfBody(AstNode *ifBody) { this->ifBody = ifBody; }

AstNode *IfStatementNode::getElseBody() { return elseBody; }

void IfStatementNode::setElseBody(AstNode *elseBody) { this->elseBody = elseBody; }
