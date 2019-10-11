#pragma once

#include "../Types.h"
#include <string>

class ASTVisitor;

class AstNode {
  public:

    explicit AstNode(ast::NodeType type) : type(type) {}

    virtual void accept(ASTVisitor *v) = 0;

    ast::NodeType getAstNodeType() { return type; }

  private:
    ast::NodeType type;
};

std::string to_string(ast::NodeType type);
