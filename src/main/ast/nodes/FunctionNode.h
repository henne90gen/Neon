#pragma once

class FunctionNode : public AstNode {
  public:
    explicit FunctionNode(std::string name, AstNode::DataType returnType)
          : AstNode(AstNode::FUNCTION), name(std::move(name)), returnType(returnType) {}

    void accept(ASTVisitor *v) override { v->visitFunctionNode(this); };

    AstNode *getBody() { return body; }
    void setBody(AstNode *body) { this->body = body; }

    std::vector<VariableDefinitionNode *> &getArguments() { return arguments; }

    std::string &getName() { return name; }

    AstNode::DataType getReturnType() { return returnType; }

    bool isExternal() { return body == nullptr; }

  private:
    std::string name;
    AstNode::DataType returnType;
    AstNode *body = nullptr;
    std::vector<VariableDefinitionNode *> arguments = {};
};

class CallNode : public AstNode {
  public:
    explicit CallNode(std::string name) : AstNode(AstNode::CALL), name(std::move(name)) {}

    void accept(ASTVisitor *v) override { v->visitCallNode(this); }

    std::string &getName() { return name; }

    std::vector<AstNode *> &getArguments() { return arguments; }

  private:
    std::string name;
    std::vector<AstNode *> arguments = {};
};
