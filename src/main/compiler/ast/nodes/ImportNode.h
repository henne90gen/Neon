#pragma once

#include "AstNode.h"

class ImportNode : public AstNode {
  public:
    explicit ImportNode() : AstNode(ast::NodeType::IMPORT) {}

    void accept(AstVisitor *v) override;

    void setFileName(std::string fileName);
    std::string getFileName();

  private:
    std::string fileName;
};
