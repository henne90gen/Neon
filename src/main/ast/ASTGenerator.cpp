#include "ASTGenerator.h"

#include <iostream>

#include "nodes/AllNodes.h"

auto isBinaryOperation(ParseTreeNode *node) -> bool {
    return (node->symbol == GrammarSymbol::SUM || node->symbol == GrammarSymbol::TERM ||
            node->symbol == GrammarSymbol::RELATION) &&
           node->children.size() == 3;
}

auto isUnaryOperation(ParseTreeNode *node) -> bool {
    return (node->symbol == GrammarSymbol::NEGATION) && node->children.size() == 2;
}

auto isLiteral(ParseTreeNode *node) -> bool {
    return node->symbol == GrammarSymbol::INTEGER || node->symbol == GrammarSymbol::FLOAT ||
           node->symbol == GrammarSymbol::TRUE || node->symbol == GrammarSymbol::FALSE;
}

auto isVariable(ParseTreeNode *node) -> bool { return node->symbol == GrammarSymbol::VARIABLE_NAME; }

auto isVariableDefinition(ParseTreeNode *node) -> bool { return node->symbol == GrammarSymbol::VARIABLE_DEFINITION; }

auto isSequence(ParseTreeNode *node) -> bool { return node->symbol == GrammarSymbol::STATEMENTS; }

auto isStatement(ParseTreeNode *node) -> bool { return node->symbol == GrammarSymbol::STATEMENT; }

auto isFunction(ParseTreeNode *node) -> bool { return node->symbol == GrammarSymbol::FUNCTION; }

auto isExternalFunction(ParseTreeNode *node) -> bool { return node->symbol == GrammarSymbol::EXTERNAL_FUNCTION; }

auto isIfStatement(ParseTreeNode *node) -> bool { return node->symbol == GrammarSymbol::IF_STATEMENT; }

auto isCall(ParseTreeNode *node) -> bool { return node->symbol == GrammarSymbol::CALL; }

auto isAssignment(ParseTreeNode *node) -> bool { return node->symbol == GrammarSymbol::ASSIGNMENT; }

auto isIgnored(ParseTreeNode *node) -> bool {
    return node->symbol == GrammarSymbol::SEMICOLON || node->symbol == GrammarSymbol::ENDOFFILE;
}

auto getBinaryOperationType(GrammarSymbol symbol) -> ast::BinaryOperationType {
    switch (symbol) {
    case GrammarSymbol::PLUS:
        return ast::BinaryOperationType::ADDITION;
    case GrammarSymbol::MINUS:
        return ast::BinaryOperationType::SUBTRACTION;
    case GrammarSymbol::STAR:
        return ast::BinaryOperationType::MULTIPLICATION;
    case GrammarSymbol::DIV:
        return ast::BinaryOperationType::DIVISION;
    case GrammarSymbol::DOUBLE_EQUALS:
        return ast::BinaryOperationType::EQUALS;
    case GrammarSymbol::NOT_EQUALS:
        return ast::BinaryOperationType::NOT_EQUALS;
    case GrammarSymbol::LESS_EQUALS:
        return ast::BinaryOperationType::LESS_EQUALS;
    case GrammarSymbol::LESS_THAN:
        return ast::BinaryOperationType::LESS_THAN;
    case GrammarSymbol::GREATER_EQUALS:
        return ast::BinaryOperationType::GREATER_EQUALS;
    case GrammarSymbol::GREATER_THAN:
        return ast::BinaryOperationType::GREATER_THAN;
    default:
        std::cout << "Could not determine binary operator type! (" << to_string(symbol) << ")" << std::endl;
        exit(1);
        return ast::BinaryOperationType::ADDITION;
    }
}

auto getUnaryOperationType(GrammarSymbol symbol) -> UnaryOperationNode::UnaryOperationType {
    switch (symbol) {
    case GrammarSymbol::NOT:
        return UnaryOperationNode::NOT;
    default:
        std::cout << "Could not determine unary operator type! (" << to_string(symbol) << ")" << std::endl;
        exit(1);
        return UnaryOperationNode::NOT;
    }
}

auto createBinaryOperation(ParseTreeNode *node) -> AstNode * {
    auto nodeType = getBinaryOperationType(node->children[1]->symbol);
    auto astNode = new BinaryOperationNode(nodeType);

    auto leftNode = createAstFromParseTree(node->children[0]);
    astNode->setLeft(leftNode);

    auto rightNode = createAstFromParseTree(node->children[2]);
    astNode->setRight(rightNode);
    return astNode;
}

auto createUnaryOperation(ParseTreeNode *node) -> AstNode * {
    auto nodeType = getUnaryOperationType(node->children[0]->symbol);
    auto unaryOperationNode = new UnaryOperationNode(nodeType);

    auto child = createAstFromParseTree(node->children[1]);
    unaryOperationNode->setChild(child);
    return unaryOperationNode;
}

auto createLiteral(ParseTreeNode *node) -> AstNode * {
    if (node->token.content.empty()) {
        std::cout << "Missing token content. Can't create a literal." << std::endl;
        return nullptr;
    }

    switch (node->symbol) {
    case GrammarSymbol::INTEGER: {
        int value = std::stoi(node->token.content);
        return new IntegerNode(value);
    }
    case GrammarSymbol::FLOAT: {
        float value = std::stof(node->token.content);
        return new FloatNode(value);
    }
    case GrammarSymbol::TRUE:
    case GrammarSymbol::FALSE: {
        bool value = false;
        if (node->token.content == "true") {
            value = true;
        }
        return new BoolNode(value);
    }
    default:
        std::cout << "Data type not supported yet!" << std::endl;
        return nullptr;
    }
}

auto createVariable(ParseTreeNode *node) -> AstNode * { return new VariableNode(node->token.content); }

auto getDataType(ParseTreeNode *node) -> ast::DataType {
    if (node->token.type != Token::DATA_TYPE) {
        return ast::DataType::VOID;
    }

    if (node->token.content == "int") {
        return ast::DataType::INT;
    }
    if (node->token.content == "float") {
        return ast::DataType::FLOAT;
    }
    if (node->token.content == "bool") {
        return ast::DataType::BOOL;
    }
    return ast::DataType::VOID;
}

auto createVariableDefinition(ParseTreeNode *node) -> VariableDefinitionNode * {
    auto dataTypeNode = node->children[0];
    auto nameNode = node->children[1];
    ast::DataType dataType = getDataType(dataTypeNode);
    return new VariableDefinitionNode(nameNode->token.content, dataType);
}

auto createSequence(ParseTreeNode *node, SequenceNode *seqRoot = nullptr) -> AstNode * {
    if (seqRoot == nullptr) {
        seqRoot = new SequenceNode();
    }

    for (auto child : node->children) {
        if (isSequence(child)) {
            seqRoot = dynamic_cast<SequenceNode *>(createSequence(child, seqRoot));
            continue;
        }
        auto astChild = createAstFromParseTree(child);
        if (astChild != nullptr) {
            seqRoot->getChildren().push_back(astChild);
        }
    }

    return seqRoot;
}

auto createStatement(ParseTreeNode *node) -> StatementNode * {
    auto statementNode = new StatementNode();
    if (node->children.empty()) {
        std::cout << "Statement did not contain anything." << std::endl;
        return statementNode;
    }

    bool isReturnStatement = node->children[0]->symbol == GrammarSymbol::RETURN;
    statementNode->setIsReturnStatement(isReturnStatement);

    if (node->children.size() > 2 && !isReturnStatement) {
        std::cout << "A statement should never have more than two children." << std::endl;
    }

    auto child = createAstFromParseTree(node->children[isReturnStatement ? 1 : 0]);
    statementNode->setChild(child);

    return statementNode;
}

void addArguments(FunctionNode *function, ParseTreeNode *root) {
    auto currentNode = root;
    while (currentNode != nullptr) {
        ParseTreeNode *argNode = nullptr;
        if (currentNode->children.size() == 1) {
            argNode = currentNode->children[0];
        } else if (currentNode->children.size() == 3) {
            argNode = currentNode->children[2];
        }

        auto argument = createVariableDefinition(argNode);
        function->getArguments().push_back(argument);

        if (currentNode->children.size() == 3) {
            currentNode = currentNode->children[0];
        } else {
            break;
        }
    }
}

auto createExternalFunction(ParseTreeNode *node) -> FunctionNode * {
    auto variableNameNode = node->children[2];
    auto header = node->children[4];
    ParseTreeNode *argumentsNode = nullptr;
    ParseTreeNode *returnNode = nullptr;
    if (header->children[0]->symbol == GrammarSymbol::FUNCTION_ARGS) {
        // we have function arguments
        argumentsNode = header->children[0];
        returnNode = header->children[1];
    } else {
        // we don't have function arguments
        returnNode = header->children[0];
    }

    ParseTreeNode *returnTypeNode = nullptr;
    if (returnNode->children.size() == 2) {
        // we have a return type
        returnTypeNode = returnNode->children[1];
    } else {
        // we don't have a return type (implicitly void)
    }

    auto returnType = ast::DataType::VOID;
    if (returnTypeNode != nullptr) {
        returnType = getDataType(returnTypeNode);
    }

    auto function = new FunctionNode(variableNameNode->token.content, returnType);
    if (argumentsNode != nullptr) {
        addArguments(function, argumentsNode);
    }
    return function;
}

auto createFunction(ParseTreeNode *node) -> FunctionNode * {
    auto variableNameNode = node->children[1];
    auto header = node->children[3];
    ParseTreeNode *argumentsNode = nullptr;
    ParseTreeNode *returnNode = nullptr;
    if (header->children[0]->symbol == GrammarSymbol::FUNCTION_ARGS) {
        // we have function arguments
        argumentsNode = header->children[0];
        returnNode = header->children[2];
    } else {
        // we don't have function arguments
        returnNode = header->children[1];
    }

    ParseTreeNode *returnTypeNode = nullptr;
    ParseTreeNode *bodyNode = nullptr;
    if (returnNode->children[0]->symbol == GrammarSymbol::DATA_TYPE) {
        // we have a return type
        returnTypeNode = returnNode->children[0];
        bodyNode = returnNode->children[2];
    } else {
        // we don't have a return type (implicitly void)
        bodyNode = returnNode->children[1];
    }

    auto returnType = ast::DataType::VOID;
    if (returnTypeNode != nullptr) {
        returnType = getDataType(returnTypeNode);
    }
    auto function = new FunctionNode(variableNameNode->token.content, returnType);
    if (argumentsNode != nullptr) {
        addArguments(function, argumentsNode);
    }

    if (bodyNode->children[0]->symbol == GrammarSymbol::RIGHT_CURLY_BRACE) {
        // empty body
        function->setBody(new SequenceNode());
    } else {
        auto body = createAstFromParseTree(bodyNode->children[0]);
        function->setBody(body);
    }

    return function;
}

auto createAssignment(ParseTreeNode *node) -> AssignmentNode * {
    auto assignment = new AssignmentNode();
    if (node->children.size() != 3) {
        std::cout << "Assignment should always have 3 children" << std::endl;
        return assignment;
    }

    auto left = createAstFromParseTree(node->children[0]);
    assignment->setLeft(left);
    auto right = createAstFromParseTree(node->children[2]);
    assignment->setRight(right);
    return assignment;
}

void addArguments(CallNode *call, ParseTreeNode *root) {
    auto currentNode = root;
    while (currentNode != nullptr) {
        ParseTreeNode *argNode = nullptr;
        if (currentNode->children.size() == 1) {
            argNode = currentNode->children[0];
        } else if (currentNode->children.size() == 3) {
            argNode = currentNode->children[2];
        }

        auto argument = createAstFromParseTree(argNode);
        call->getArguments().push_back(argument);

        if (currentNode->children.size() == 3) {
            currentNode = currentNode->children[0];
        } else {
            break;
        }
    }
}

auto createCall(ParseTreeNode *node) -> CallNode * {
    std::string name = node->children[0]->token.content;
    auto call = new CallNode(name);
    auto call_header = node->children[2];
    if (call_header->children.size() == 2) {
        addArguments(call, call_header->children[0]);
    }
    return call;
}

auto createIfStatement(ParseTreeNode *node) -> IfStatementNode * {
    auto ifNode = new IfStatementNode();
    AstNode *condition = createAstFromParseTree(node->children[1]);
    ifNode->setCondition(condition);
    auto ifBodyNode = node->children[3];
    auto elseNode = ifBodyNode->children[0];
    if (ifBodyNode->children.size() == 2) {
        ifNode->setIfBody(createAstFromParseTree(ifBodyNode->children[0]));
        elseNode = ifBodyNode->children[1];
    }
    if (elseNode->children.size() == 4) {
        auto elseBodyNode = elseNode->children[3];
        if (elseBodyNode->children.size() == 2) {
            ifNode->setElseBody(createAstFromParseTree(elseBodyNode->children[0]));
        }
    }
    return ifNode;
}

auto createAstFromParseTree(ParseTreeNode *node) -> AstNode * {
    if (node == nullptr) {
        return nullptr;
    }

    if (isBinaryOperation(node)) {
        return createBinaryOperation(node);
    }

    if (isUnaryOperation(node)) {
        return createUnaryOperation(node);
    }

    if (isLiteral(node)) {
        return createLiteral(node);
    }

    if (isSequence(node)) {
        return createSequence(node);
    }

    if (isStatement(node)) {
        return createStatement(node);
    }

    if (isVariable(node)) {
        return createVariable(node);
    }

    if (isVariableDefinition(node)) {
        return createVariableDefinition(node);
    }

    if (isFunction(node)) {
        return createFunction(node);
    }

    if (isExternalFunction(node)) {
        return createExternalFunction(node);
    }

    if (isIfStatement(node)) {
        return createIfStatement(node);
    }

    if (isCall(node)) {
        return createCall(node);
    }

    if (isAssignment(node)) {
        return createAssignment(node);
    }

    if (node->children.size() == 1 || node->symbol == GrammarSymbol::PROGRAM) {
        return createAstFromParseTree(node->children[0]);
    }

    if (node->symbol == GrammarSymbol::FACTOR && node->children.size() == 3 &&
        node->children[0]->symbol == GrammarSymbol::LEFT_PARAN &&
        node->children[2]->symbol == GrammarSymbol::RIGHT_PARAN) {
        // removes parantheses from expressions
        return createAstFromParseTree(node->children[1]);
    }

    if (isIgnored(node)) {
        return nullptr;
    }

    std::cout << "Could not find a suitable AstNode for " << to_string(node->symbol) << std::endl;

    return nullptr;
}
