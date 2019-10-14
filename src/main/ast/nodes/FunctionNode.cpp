#include "FunctionNode.h"

#include "../ASTVisitor.h"

void FunctionNode::accept(ASTVisitor *v) { v->visitFunctionNode(this); }

auto FunctionNode::getBody() -> AstNode * { return body; }

void FunctionNode::setBody(AstNode *body) { this->body = body; }

auto FunctionNode::getArguments() -> std::vector<VariableDefinitionNode *> & { return arguments; }

auto FunctionNode::getName() -> std::string & { return name; }

auto FunctionNode::getReturnType() -> ast::DataType { return returnType; }

auto FunctionNode::isExternal() -> bool { return body == nullptr; }

void CallNode::accept(ASTVisitor *v) { v->visitCallNode(this); }

auto CallNode::getName() -> std::string & { return name; }

auto CallNode::getArguments() -> std::vector<AstNode *> & { return arguments; }
