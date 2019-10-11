#include "FunctionNode.h"

#include "../ASTVisitor.h"

void FunctionNode::accept(ASTVisitor *v) { v->visitFunctionNode(this); }

AstNode *FunctionNode::getBody() { return body; }

void FunctionNode::setBody(AstNode *body) { this->body = body; }

std::vector<VariableDefinitionNode *> &FunctionNode::getArguments() { return arguments; }

std::string &FunctionNode::getName() { return name; }

ast::DataType FunctionNode::getReturnType() { return returnType; }

bool FunctionNode::isExternal() { return body == nullptr; }

void CallNode::accept(ASTVisitor *v) { v->visitCallNode(this); }

std::string &CallNode::getName() { return name; }

std::vector<AstNode *> &CallNode::getArguments() { return arguments; }
