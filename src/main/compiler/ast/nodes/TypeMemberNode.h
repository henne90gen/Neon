#pragma once

#include <utility>

#include "AstNode.h"

class VariableDefinitionNode;
class TypeMemberNode : public AstNode {
  public:
    explicit TypeMemberNode(VariableDefinitionNode *inner) : AstNode(ast::NodeType::TYPE_MEMBER), inner(inner) {}

    void accept(AstVisitor *v) override;

    VariableDefinitionNode *getDefinition() { return inner; }

  private:
    VariableDefinitionNode *inner = nullptr;
};
