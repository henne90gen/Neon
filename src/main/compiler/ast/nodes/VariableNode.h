#pragma once

#include "AstNode.h"

class VariableNode : public AstNode {
  public:
    explicit VariableNode(std::string name);

    void accept(AstVisitor *v) override;

    std::string &getName();

    bool isArrayAccess();

    AstNode* getArrayIndex();

    void setArrayIndex(AstNode* arrayIndex);

  private:
    std::string name;

    AstNode* arrayIndex = nullptr;
};
