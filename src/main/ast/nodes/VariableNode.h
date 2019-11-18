#pragma once

#include "AstNode.h"

class VariableNode : public AstNode {
  public:
    explicit VariableNode(std::string name) : AstNode(ast::NodeType::VARIABLE), name(std::move(name)) {}

    void accept(AstVisitor *v) override;

    std::string &getName() { return name; }

    bool isArrayAccess() { return arrayAccess; }

    int getArrayIndex() { return this->arrayIndex; }
    void setArrayIndex(int arrayIndex) {
        this->arrayIndex = arrayIndex;
        this->arrayAccess = true;
    }

  private:
    std::string name;

    bool arrayAccess = false;
    // TODO should indices that are too large, wrap around to the beginning
    int arrayIndex = 0; // negative array indices wrap around the end
};
