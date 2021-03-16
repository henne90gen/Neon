#pragma once

#include <utility>

#include "AstNode.h"

class CommentNode : public AstNode {
  public:
    explicit CommentNode(std::string content) : AstNode(ast::NodeType::COMMENT), content(std::move(content)) {}

    void accept(AstVisitor *v) override;

    [[nodiscard]] const std::string &getContent() const;

  private:
    std::string content;
};
