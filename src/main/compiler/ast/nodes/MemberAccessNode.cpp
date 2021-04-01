#include "MemberAccessNode.h"

#include "../AstVisitor.h"
#include "VariableNode.h"

void MemberAccessNode::accept(AstVisitor *visitor) { visitor->visitMemberAccessNode(this); }

std::vector<VariableNode *> MemberAccessNode::linearizeAccessTree() {
    std::vector<VariableNode *> variables = {};
    std::vector<AstNode *> stack = {getRight(), getLeft()};
    while (!stack.empty()) {
        auto *current = stack.back();
        stack.pop_back();
        if (current->getAstNodeType() == ast::NodeType::VARIABLE) {
            variables.push_back(reinterpret_cast<VariableNode *>(current));
            continue;
        } if (current->getAstNodeType() == ast::NodeType::MEMBER_ACCESS) {
            auto *ma = reinterpret_cast<MemberAccessNode *>(current);
            stack.push_back(ma->getRight());
            stack.push_back(ma->getLeft());
        } else {
            // empty return value signals an error
            return {};
        }
    }
    return variables;
}

std::string MemberAccessNode::toString() {
    auto variables = linearizeAccessTree();
    std::string result;
    for (auto variable : variables) {
        result += variable->getName() + ".";
    }
    return result.substr(0, result.size() - 1);
}
