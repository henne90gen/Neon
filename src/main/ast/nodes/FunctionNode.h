#pragma once

#include "AstNode.h"
#include "VariableDefinitionNode.h"

#include <vector>

class FunctionNode : public AstNode {
  public:
    explicit FunctionNode(std::string name, ast::DataType returnType)
          : AstNode(ast::NodeType::FUNCTION), name(std::move(name)), returnType(returnType) {}

    void accept(ASTVisitor *v) override;

    AstNode *getBody();
    void setBody(AstNode *body);

    std::vector<VariableDefinitionNode *> &getArguments();

    std::string &getName();

    ast::DataType getReturnType();

    bool isExternal();

  private:
    std::string name;
    ast::DataType returnType;
    AstNode *body = nullptr;
    std::vector<VariableDefinitionNode *> arguments = {};
};

class CallNode : public AstNode {
  public:
    explicit CallNode(std::string name) : AstNode(ast::NodeType::CALL), name(std::move(name)) {}

    void accept(ASTVisitor *v) override;

    std::string &getName();

    std::vector<AstNode *> &getArguments();

  private:
    std::string name;
    std::vector<AstNode *> arguments = {};
};
