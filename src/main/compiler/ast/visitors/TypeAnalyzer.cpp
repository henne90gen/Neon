#include "TypeAnalyzer.h"

#include "util/Utils.h"

void TypeAnalyzer::visitFunctionNode(FunctionNode *node) {
    for (auto &argument : node->arguments) {
        visitNode(AST_NODE(argument));
    }
    if (!node->is_external()) {
        visitNode(node->body);
    }
}

void TypeAnalyzer::visitCallNode(CallNode *node) {
    auto result = functionResolver.resolveFunction(module, node->name);
    if (!result.functionExists) {
        std::cerr << "TypeAnalyzer: Undefined function " << node->name << std::endl;
        return;
    }
    for (auto *const arg : node->arguments) {
        visitNode(arg);
    }
    nodeTypeMap[AST_NODE(node)] = result.signature.returnType;
}

void TypeAnalyzer::visitVariableNode(VariableNode *node) {
    const auto &itr = variableTypeMap.find(node->name);
    if (itr == variableTypeMap.end()) {
        std::cerr << "TypeAnalyzer: Undefined variable " << node->name << std::endl;
        return;
    }
    nodeTypeMap[AST_NODE(node)] = itr->second;
}

void TypeAnalyzer::visitVariableDefinitionNode(VariableDefinitionNode *node) {
    nodeTypeMap[AST_NODE(node)] = node->type;
    variableTypeMap[node->name] = node->type;
}

void TypeAnalyzer::visitBinaryOperationNode(BinaryOperationNode *node) {
    visitNode(node->left);
    visitNode(node->right);
    auto leftType = nodeTypeMap[node->left];
    auto rightType = nodeTypeMap[node->right];
    if (leftType == rightType) {
        if (node->type == ast::BinaryOperationType::ADDITION || node->type == ast::BinaryOperationType::SUBTRACTION ||
            node->type == ast::BinaryOperationType::MULTIPLICATION ||
            node->type == ast::BinaryOperationType::DIVISION) {
            nodeTypeMap[AST_NODE(node)] = leftType;
        } else if (node->type == ast::BinaryOperationType::LESS_EQUALS ||
                   node->type == ast::BinaryOperationType::LESS_THAN ||
                   node->type == ast::BinaryOperationType::GREATER_EQUALS ||
                   node->type == ast::BinaryOperationType::GREATER_THAN ||
                   node->type == ast::BinaryOperationType::EQUALS ||
                   node->type == ast::BinaryOperationType::NOT_EQUALS) {
            nodeTypeMap[AST_NODE(node)] = ast::DataType(ast::SimpleDataType::BOOLEAN);
        }
        return;
    }
    std::cerr << "TypeAnalyzer: Binary operation type mismatch: " << to_string(leftType) << " " << to_string(node->type)
              << " " << to_string(rightType) << std::endl;
}

void TypeAnalyzer::visitUnaryOperationNode(UnaryOperationNode *node) {
    visitNode(node->child);
    if (node->type == ast::UnaryOperationType::NOT &&
        nodeTypeMap[node->child] == ast::DataType(ast::SimpleDataType::BOOLEAN)) {
        nodeTypeMap[AST_NODE(node)] = ast::DataType(ast::SimpleDataType::BOOLEAN);
        return;
    }
    if (node->type == ast::UnaryOperationType::NEGATE) {
        if (nodeTypeMap[node->child] == ast::DataType(ast::SimpleDataType::INTEGER)) {
            nodeTypeMap[AST_NODE(node)] = ast::DataType(ast::SimpleDataType::INTEGER);
            return;
        }
        if (nodeTypeMap[node->child] == ast::DataType(ast::SimpleDataType::FLOAT)) {
            nodeTypeMap[AST_NODE(node)] = ast::DataType(ast::SimpleDataType::FLOAT);
            return;
        }
    }
    std::cerr << "TypeAnalyzer: Unary operation type mismatch: " << to_string(node->type) << std::endl;
}

void TypeAnalyzer::visitAssignmentNode(AssignmentNode *node) {
    visitNode(node->right);
    visitNode(node->left);
    ast::DataType leftType = nodeTypeMap[node->left];
    ast::DataType rightType = nodeTypeMap[node->right];
    if (leftType != rightType) {
        std::cerr << "TypeAnalyzer: Assignment type mismatch: " << to_string(leftType) << " = " << to_string(rightType)
                  << std::endl;
        return;
    }

    nodeTypeMap[AST_NODE(node)] = leftType;
}

void TypeAnalyzer::visitAssertNode(AssertNode *node) {
    visitNode(node->condition);
    nodeTypeMap[AST_NODE(node)] = nodeTypeMap[node->condition];
}

void TypeAnalyzer::visitSequenceNode(SequenceNode *node) {
    for (auto *child : node->children) {
        visitNode(child);
    }
}

void TypeAnalyzer::visitStatementNode(StatementNode *node) {
    if (node->child == nullptr) {
        return;
    }
    visitNode(node->child);
    nodeTypeMap[AST_NODE(node)] = nodeTypeMap[node->child];
}

void TypeAnalyzer::visitLiteralNode(LiteralNode *node) {
    switch (node->type) {
    case LiteralType::BOOL:
        nodeTypeMap[AST_NODE(node)] = ast::DataType(ast::SimpleDataType::BOOLEAN);
        break;
    case LiteralType::INTEGER:
        nodeTypeMap[AST_NODE(node)] = ast::DataType(ast::SimpleDataType::INTEGER);
        break;
    case LiteralType::FLOAT:
        nodeTypeMap[AST_NODE(node)] = ast::DataType(ast::SimpleDataType::FLOAT);
        break;
    case LiteralType::STRING:
        nodeTypeMap[AST_NODE(node)] = ast::DataType(ast::SimpleDataType::STRING);
        break;
    }
}

void TypeAnalyzer::visitIfStatementNode(IfStatementNode *node) {
    visitNode(node->condition);
    if (nodeTypeMap[node->condition] != ast::DataType(ast::SimpleDataType::BOOLEAN)) {
        std::cerr << "If condition is not of type bool" << std::endl;
        return;
    }
    if (node->ifBody != nullptr) {
        visitNode(node->ifBody);
    }
    if (node->elseBody != nullptr) {
        visitNode(node->elseBody);
    }
}

void TypeAnalyzer::visitForStatementNode(ForStatementNode *node) {
    if (node->init != nullptr) {
        visitNode(node->init);
    }

    visitNode(node->condition);
    if (nodeTypeMap[node->condition] != ast::DataType(ast::SimpleDataType::BOOLEAN)) {
        std::cerr << "For condition is not of type bool" << std::endl;
        return;
    }

    if (node->update != nullptr) {
        visitNode(node->update);
    }

    if (node->body != nullptr) {
        visitNode(node->body);
    }
}

void TypeAnalyzer::visitTypeDeclarationNode(TypeDeclarationNode *node) {
    std::vector<ComplexTypeMember> members = {};
    for (const auto &member : node->members) {
        ComplexTypeMember m = {
              member->variable_definition->name,
              member->variable_definition->type,
        };
        members.push_back(m);
    }
    complexTypeMap[node->type()] = {node->type(), members};
}

void TypeAnalyzer::visitMemberAccessNode(MemberAccessNode *node) {
    auto variables = node->linearize_access_tree();
    if (variables.empty()) {
        log.error("Failed to linearize MemberAccess tree");
        return;
    }

    auto &variableType = variableTypeMap[variables[0]->name];
    auto currentType = complexTypeMap[variableType];
    nodeTypeMap[AST_NODE(variables[0])] = currentType.type;
    for (int i = 1; i < variables.size(); i++) {
        auto *variable = variables[i];
        for (const auto &member : currentType.members) {
            if (member.name != variable->name) {
                continue;
            }

            if (ast::isSimpleDataType(member.type)) {
                nodeTypeMap[AST_NODE(node)] = member.type;
                nodeTypeMap[AST_NODE(variable)] = member.type;
                break;
            }

            currentType = complexTypeMap[member.type];
            nodeTypeMap[AST_NODE(variable)] = currentType.type;
        }
    }
}

std::pair<std::unordered_map<AstNode *, ast::DataType>, std::unordered_map<std::string, ast::DataType>>
TypeAnalyzer::run(AST &tree) {
    visitNode(tree.root());
    return std::make_pair(nodeTypeMap, variableTypeMap);
}

void TypeAnalyzer::visitNode(AstNode *node) {
    switch (node->type) {
    case ast::NodeType::SEQUENCE:
        visitSequenceNode(&node->sequence);
        break;
    case ast::NodeType::STATEMENT:
        visitStatementNode(&node->statement);
        break;
    case ast::NodeType::LITERAL:
        visitLiteralNode(&node->literal);
        break;
    case ast::NodeType::UNARY_OPERATION:
        visitUnaryOperationNode(&node->unary_operation);
        break;
    case ast::NodeType::BINARY_OPERATION:
        visitBinaryOperationNode(&node->binary_operation);
        break;
    case ast::NodeType::FUNCTION:
        visitFunctionNode(&node->function);
        break;
    case ast::NodeType::CALL:
        visitCallNode(&node->call);
        break;
    case ast::NodeType::VARIABLE_DEFINITION:
        visitVariableDefinitionNode(&node->variable_definition);
        break;
    case ast::NodeType::VARIABLE:
        visitVariableNode(&node->variable);
        break;
    case ast::NodeType::ASSIGNMENT:
        visitAssignmentNode(&node->assignment);
        break;
    case ast::NodeType::IF_STATEMENT:
        visitIfStatementNode(&node->if_statement);
        break;
    case ast::NodeType::FOR_STATEMENT:
        visitForStatementNode(&node->for_statement);
        break;
    case ast::NodeType::TYPE_DECLARATION:
        visitTypeDeclarationNode(&node->type_declaration);
        break;
    case ast::NodeType::MEMBER_ACCESS:
        visitMemberAccessNode(&node->member_access);
        break;
    case ast::NodeType::ASSERT:
        visitAssertNode(&node->assert);
        break;
    case ast::NodeType::TYPE_MEMBER:
    case ast::NodeType::IMPORT:
    case ast::NodeType::COMMENT:
        // do nothing
        break;
    }
}
