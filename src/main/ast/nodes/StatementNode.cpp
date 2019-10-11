#include "StatementNode.h"

#include "../ASTVisitor.h"

void StatementNode::accept(ASTVisitor *v) { v->visitStatementNode(this); }

bool StatementNode::isReturnStatement() { return this->returnStatement; }

void StatementNode::setIsReturnStatement(bool returnStatement) { this->returnStatement = returnStatement; }

AstNode *StatementNode::getChild() { return child; }

void StatementNode::setChild(AstNode *child) { this->child = child; }
