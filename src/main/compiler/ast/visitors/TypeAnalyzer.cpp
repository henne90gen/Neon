#include "TypeAnalyzer.h"

#include <iostream>

#include "../nodes/AllNodes.h"
#include "util/Utils.h"

void TypeAnalyzer::visitFunctionNode(FunctionNode *node) {
    for (auto &argument : node->getArguments()) {
        argument->accept(this);
    }
    if (!node->isExternal()) {
        node->getBody()->accept(this);
    }
}

void TypeAnalyzer::visitCallNode(CallNode *node) {
    auto result = functionResolver.resolveFunction(module, node->getName());
    if (!result.functionExists) {
        std::cerr << "TypeAnalyzer: Undefined function " << node->getName() << std::endl;
        return;
    }
    for (const auto arg : node->getArguments()) {
        arg->accept(this);
    }
    nodeTypeMap[node] = result.signature.returnType;
}

void TypeAnalyzer::visitVariableNode(VariableNode *node) {
    const auto &itr = variableTypeMap.find(node->getName());
    if (itr == variableTypeMap.end()) {
        std::cerr << "TypeAnalyzer: Undefined variable " << node->getName() << std::endl;
        return;
    }
    nodeTypeMap[node] = itr->second;
}

void TypeAnalyzer::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    nodeTypeMap[node] = node->getType();
    variableTypeMap[node->getName()] = node->getType();
}

void TypeAnalyzer::visitBinaryOperationNode(BinaryOperationNode *node) {
    node->getLeft()->accept(this);
    node->getRight()->accept(this);
    auto leftType = nodeTypeMap[node->getLeft()];
    auto rightType = nodeTypeMap[node->getRight()];
    if (leftType == rightType) {
        if (node->getType() == ast::BinaryOperationType::ADDITION ||
            node->getType() == ast::BinaryOperationType::SUBTRACTION ||
            node->getType() == ast::BinaryOperationType::MULTIPLICATION ||
            node->getType() == ast::BinaryOperationType::DIVISION) {
            nodeTypeMap[node] = leftType;
        } else if (node->getType() == ast::BinaryOperationType::LESS_EQUALS ||
                   node->getType() == ast::BinaryOperationType::LESS_THAN ||
                   node->getType() == ast::BinaryOperationType::GREATER_EQUALS ||
                   node->getType() == ast::BinaryOperationType::GREATER_THAN ||
                   node->getType() == ast::BinaryOperationType::EQUALS ||
                   node->getType() == ast::BinaryOperationType::NOT_EQUALS) {
            nodeTypeMap[node] = ast::DataType(ast::SimpleDataType::BOOL);
        }
        return;
    }
    std::cerr << "TypeAnalyzer: Binary operation type mismatch: " << to_string(node->getAstNodeType()) << std::endl;
}

void TypeAnalyzer::visitUnaryOperationNode(UnaryOperationNode *node) {
    node->getChild()->accept(this);
    if (node->getType() == ast::UnaryOperationType::NOT &&
        nodeTypeMap[node->getChild()] == ast::DataType(ast::SimpleDataType::BOOL)) {
        nodeTypeMap[node] = ast::DataType(ast::SimpleDataType::BOOL);
        return;
    }
    if (node->getType() == ast::UnaryOperationType::NEGATE) {
        if (nodeTypeMap[node->getChild()] == ast::DataType(ast::SimpleDataType::INT)) {
            nodeTypeMap[node] = ast::DataType(ast::SimpleDataType::INT);
            return;
        }
        if (nodeTypeMap[node->getChild()] == ast::DataType(ast::SimpleDataType::FLOAT)) {
            nodeTypeMap[node] = ast::DataType(ast::SimpleDataType::FLOAT);
            return;
        }
    }
    // TODO(henne): unary operators can also be of other types than bool, we need to add support for that as well
    std::cerr << "TypeAnalyzer: Unary operation type mismatch: " << to_string(node->getType()) << std::endl;
}

void TypeAnalyzer::visitAssignmentNode(AssignmentNode *node) {
    node->getRight()->accept(this);
    node->getLeft()->accept(this);
    ast::DataType leftType = nodeTypeMap[node->getLeft()];
    ast::DataType rightType = nodeTypeMap[node->getRight()];
    if (leftType != rightType) {
        std::cerr << "TypeAnalyzer: Assignment type mismatch: " << to_string(leftType) << " = " << to_string(rightType)
                  << std::endl;
        return;
    }

    nodeTypeMap[node] = leftType;
}

void TypeAnalyzer::visitAssertNode(AssertNode *node) {
    node->getCondition()->accept(this);
    nodeTypeMap[node] = nodeTypeMap[node->getCondition()];
}

void TypeAnalyzer::visitSequenceNode(SequenceNode *node) {
    for (auto child : node->getChildren()) {
        child->accept(this);
    }
}

void TypeAnalyzer::visitStatementNode(StatementNode *node) {
    if (node->getChild() == nullptr) {
        return;
    }
    node->getChild()->accept(this);
    nodeTypeMap[node] = nodeTypeMap[node->getChild()];
}

void TypeAnalyzer::visitBoolNode(BoolNode *node) { nodeTypeMap[node] = ast::DataType(ast::SimpleDataType::BOOL); }

void TypeAnalyzer::visitFloatNode(FloatNode *node) { nodeTypeMap[node] = ast::DataType(ast::SimpleDataType::FLOAT); }

void TypeAnalyzer::visitIntegerNode(IntegerNode *node) { nodeTypeMap[node] = ast::DataType(ast::SimpleDataType::INT); }

void TypeAnalyzer::visitStringNode(StringNode *node) { nodeTypeMap[node] = ast::DataType(ast::SimpleDataType::STRING); }

void TypeAnalyzer::visitIfStatementNode(IfStatementNode *node) {
    node->getCondition()->accept(this);
    if (nodeTypeMap[node->getCondition()] != ast::DataType(ast::SimpleDataType::BOOL)) {
        std::cerr << "If condition is not of type bool" << std::endl;
        return;
    }
    if (node->getIfBody() != nullptr) {
        node->getIfBody()->accept(this);
    }
    if (node->getElseBody() != nullptr) {
        node->getElseBody()->accept(this);
    }
}

void TypeAnalyzer::visitForStatementNode(ForStatementNode *node) {
    if (node->getInit() != nullptr) {
        node->getInit()->accept(this);
    }

    node->getCondition()->accept(this);
    if (nodeTypeMap[node->getCondition()] != ast::DataType(ast::SimpleDataType::BOOL)) {
        std::cerr << "For condition is not of type bool" << std::endl;
        return;
    }

    if (node->getUpdate() != nullptr) {
        node->getUpdate()->accept(this);
    }

    if (node->getBody() != nullptr) {
        node->getBody()->accept(this);
    }
}

void TypeAnalyzer::visitTypeDeclarationNode(TypeDeclarationNode *node) {
    std::vector<ComplexTypeMember> members = {};
    for (const auto &member : node->getMembers()) {
        ComplexTypeMember m = {member->getName(), member->getType()};
        members.push_back(m);
    }
    complexTypeMap[node->getType()] = {node->getType(), members};
}

void TypeAnalyzer::visitMemberAccessNode(MemberAccessNode *node) {
    // FIXME implement MemberAccess again
#if 0
    // TODO do error handling for both of these lookups
    const auto &variableType = variableTypeMap[node->getVariableName()];
    const auto &complexType = complexTypeMap[variableType];
    // TODO eventually this lookup has to support nested access
    for (const auto &member : complexType.members) {
        if (member.name != node->getMemberAccesses()[0]) {
            continue;
        }

        if (ast::isSimpleDataType(member.type)) {
            nodeTypeMap[node] = member.type;
        } else {
            // TODO nested lookup
        }
        break;
    }
#endif
}

std::pair<std::unordered_map<AstNode *, ast::DataType>, std::unordered_map<std::string, ast::DataType>>
TypeAnalyzer::run(AstNode *root) {
    if (root == nullptr) {
        return {};
    }
    root->accept(this);
    return std::make_pair(nodeTypeMap, variableTypeMap);
}
