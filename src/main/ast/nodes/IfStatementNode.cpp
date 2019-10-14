#include "IfStatementNode.h"

#include "../ASTVisitor.h"

void IfStatementNode::accept(ASTVisitor *v) { v->visitIfStatementNode(this); }

auto IfStatementNode::getCondition() -> AstNode * { return condition; }

void IfStatementNode::setCondition(AstNode *condition) { this->condition = condition; }

auto IfStatementNode::getIfBody() -> AstNode * { return ifBody; }

void IfStatementNode::setIfBody(AstNode *ifBody) { this->ifBody = ifBody; }

auto IfStatementNode::getElseBody() -> AstNode * { return elseBody; }

void IfStatementNode::setElseBody(AstNode *elseBody) { this->elseBody = elseBody; }
