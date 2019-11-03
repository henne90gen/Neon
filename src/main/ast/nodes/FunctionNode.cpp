#include "FunctionNode.h"

#include "../AstVisitor.h"

void FunctionNode::accept(AstVisitor *v) { v->visitFunctionNode(this); }

AstNode *FunctionNode::getBody() { return body; }

void FunctionNode::setBody(AstNode *body) { this->body = body; }

std::vector<VariableDefinitionNode *> &FunctionNode::getArguments() { return arguments; }

std::string &FunctionNode::getName() { return name; }

ast::DataType FunctionNode::getReturnType() { return returnType; }

bool FunctionNode::isExternal() { return body == nullptr; }

void CallNode::accept(AstVisitor *v) { v->visitCallNode(this); }

std::string &CallNode::getName() { return name; }

std::vector<AstNode *> &CallNode::getArguments() { return arguments; }
