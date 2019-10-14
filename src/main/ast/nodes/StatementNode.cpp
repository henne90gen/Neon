#include "StatementNode.h"

#include "../ASTVisitor.h"

void StatementNode::accept(ASTVisitor *v) { v->visitStatementNode(this); }

auto StatementNode::isReturnStatement() -> bool { return this->returnStatement; }

void StatementNode::setIsReturnStatement(bool returnStatement) { this->returnStatement = returnStatement; }

auto StatementNode::getChild() -> AstNode * { return child; }

void StatementNode::setChild(AstNode *child) { this->child = child; }
