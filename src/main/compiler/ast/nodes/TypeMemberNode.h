#pragma once

#include <utility>

#include "AstNode.h"

class TypeMemberNode : public AstNode {
  public:
    explicit TypeMemberNode(std::string name, ast::DataType dataType)
        : AstNode(ast::NodeType::TYPE_MEMBER), name(std::move(name)), dataType(std::move(dataType)) {}

    void accept(AstVisitor *v) override;

    [[nodiscard]] std::string getName() const { return name; }
    ast::DataType getType() const { return dataType; }

  private:
    std::string name;
    ast::DataType dataType;
};
