#pragma once

#include <utility>
#include <vector>

#include "AstNode.h"
#include "TypeMemberNode.h"

class TypeDeclarationNode : public AstNode {
  public:
    explicit TypeDeclarationNode(std::string name) : AstNode(ast::NodeType::TYPE_DECLARATION), name(std::move(name)) {}

    void accept(AstVisitor *v) override;

    void setMembers(std::vector<TypeMemberNode *> members_) { this->members = std::move(members_); }

    [[nodiscard]] std::string getName() const { return name; }
    [[nodiscard]] std::vector<TypeMemberNode *> getMembers() const { return members; }

    ast::DataType getType() const { return ast::DataType(name); }

  private:
    std::string name;
    std::vector<TypeMemberNode *> members = {};
};
