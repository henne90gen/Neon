#include "StatementNode.h"

#include "../AstVisitor.h"

void StatementNode::accept(AstVisitor *v) { v->visitStatementNode(this); }

bool StatementNode::isReturnStatement() { return this->returnStatement; }

void StatementNode::setIsReturnStatement(bool returnStatement) { this->returnStatement = returnStatement; }

AstNode *StatementNode::getChild() { return child; }

void StatementNode::setChild(AstNode *child) { this->child = child; }
