#pragma once

#include "AstNode.h"

class VariableNode : public AstNode {
  public:
    explicit VariableNode(std::string name);

    void accept(AstVisitor *v) override;

    std::string &getName();

    bool isArrayAccess();

    int getArrayIndex();

    void setArrayIndex(int arrayIndex);

  private:
    std::string name;

    bool arrayAccess = false;
    // TODO should indices that are too large, wrap around to the beginning?
    int arrayIndex = 0; // negative array indices wrap around the end
};
