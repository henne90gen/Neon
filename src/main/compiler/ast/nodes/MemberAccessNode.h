#pragma once

#include <string>
#include <utility>
#include <vector>

#include "AstNode.h"

class MemberAccessNode : public AstNode {
  public:
    explicit MemberAccessNode(std::vector<std::string> members)
        : AstNode(ast::NodeType::MEMBER_ACCESS), parts(std::move(members)) {}

    void accept(AstVisitor *visitor) override;

    [[nodiscard]] std::string getVariableName() const { return parts[0]; }

    [[nodiscard]] std::vector<std::string> getMemberAccesses() const {
        if (parts.size() <= 1) {
            // this is not a real member access: 'myVariable.'
            return std::vector<std::string>();
        }
        return std::vector(parts.begin() + 1, parts.end());
    }

  private:
    std::vector<std::string> parts = {};
};
