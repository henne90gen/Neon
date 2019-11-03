#include "ASTGenerator.h"

#include <iostream>

#include "nodes/AllNodes.h"

bool isBinaryOperation(ParseTreeNode *node) {
    return (node->symbol == GrammarSymbol::SUM || node->symbol == GrammarSymbol::TERM ||
            node->symbol == GrammarSymbol::RELATION) &&
           node->children.size() == 3;
}

bool isUnaryOperation(ParseTreeNode *node) {
    return (node->symbol == GrammarSymbol::NEGATION) && node->children.size() == 2;
}

bool isLiteral(ParseTreeNode *node) {
    return node->symbol == GrammarSymbol::INTEGER || node->symbol == GrammarSymbol::FLOAT ||
           node->symbol == GrammarSymbol::TRUE || node->symbol == GrammarSymbol::FALSE;
}

bool isVariable(ParseTreeNode *node) { return node->symbol == GrammarSymbol::VARIABLE_NAME; }

bool isVariableDefinition(ParseTreeNode *node) { return node->symbol == GrammarSymbol::VARIABLE_DEFINITION; }

bool isSequence(ParseTreeNode *node) { return node->symbol == GrammarSymbol::STATEMENTS; }

bool isStatement(ParseTreeNode *node) { return node->symbol == GrammarSymbol::STATEMENT; }

bool isFunction(ParseTreeNode *node) { return node->symbol == GrammarSymbol::FUNCTION; }

bool isExternalFunction(ParseTreeNode *node) { return node->symbol == GrammarSymbol::EXTERNAL_FUNCTION; }

bool isIfStatement(ParseTreeNode *node) { return node->symbol == GrammarSymbol::IF_STATEMENT; }

bool isCall(ParseTreeNode *node) { return node->symbol == GrammarSymbol::CALL; }

bool isAssignment(ParseTreeNode *node) { return node->symbol == GrammarSymbol::ASSIGNMENT; }

bool isForStatement(ParseTreeNode *node) { return node->symbol == GrammarSymbol::FOR_STATEMENT; }

bool isIgnored(ParseTreeNode *node) {
    return node->symbol == GrammarSymbol::SEMICOLON || node->symbol == GrammarSymbol::ENDOFFILE;
}

ast::BinaryOperationType getBinaryOperationType(GrammarSymbol symbol) {
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

UnaryOperationNode::UnaryOperationType getUnaryOperationType(GrammarSymbol symbol) {
    switch (symbol) {
    case GrammarSymbol::NOT:
        return UnaryOperationNode::NOT;
    default:
        std::cout << "Could not determine unary operator type! (" << to_string(symbol) << ")" << std::endl;
        exit(1);
        return UnaryOperationNode::NOT;
    }
}

AstNode *createBinaryOperation(ParseTreeNode *node) {
    auto nodeType = getBinaryOperationType(node->children[1]->symbol);
    auto astNode = new BinaryOperationNode(nodeType);

    auto leftNode = createAstFromParseTree(node->children[0]);
    astNode->setLeft(leftNode);

    auto rightNode = createAstFromParseTree(node->children[2]);
    astNode->setRight(rightNode);
    return astNode;
}

AstNode *createUnaryOperation(ParseTreeNode *node) {
    auto nodeType = getUnaryOperationType(node->children[0]->symbol);
    auto unaryOperationNode = new UnaryOperationNode(nodeType);

    auto child = createAstFromParseTree(node->children[1]);
    unaryOperationNode->setChild(child);
    return unaryOperationNode;
}

AstNode *createLiteral(ParseTreeNode *node) {
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

AstNode *createVariable(ParseTreeNode *node) { return new VariableNode(node->token.content); }

ast::DataType getDataType(ParseTreeNode *node) {
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

VariableDefinitionNode *createVariableDefinition(ParseTreeNode *node) {
    auto dataTypeNode = node->children[0];
    auto nameNode = node->children[1];
    ast::DataType dataType = getDataType(dataTypeNode);
    return new VariableDefinitionNode(nameNode->token.content, dataType);
}

AstNode *createSequence(ParseTreeNode *node, SequenceNode *seqRoot) {
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

StatementNode *createStatement(ParseTreeNode *node) {
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

FunctionNode *createExternalFunction(ParseTreeNode *node) {
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

FunctionNode *createFunction(ParseTreeNode *node) {
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

AssignmentNode *createAssignment(ParseTreeNode *node) {
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

CallNode *createCall(ParseTreeNode *node) {
    std::string name = node->children[0]->token.content;
    auto call = new CallNode(name);
    auto call_header = node->children[2];
    if (call_header->children.size() == 2) {
        addArguments(call, call_header->children[0]);
    }
    return call;
}

IfStatementNode *createIfStatement(ParseTreeNode *node) {
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

ForStatementNode *createForStatement(ParseTreeNode *node) {
    // FOR
    // ASSIGNMENT
    // SEMICOLON
    // EXPRESSION
    // SEMICOLON
    // ASSIGNMENT
    // LEFT_CURLY_BRACE
    // FOR_STATEMENT_BODY
    //     STATEMENTS
    //     RIGHT_CURLY_BRACE
    auto result = new ForStatementNode();

    auto init = node->children[1];
    result->setInit(createAstFromParseTree(init));

    auto condition = node->children[3];
    result->setCondition(createAstFromParseTree(condition));

    auto update = node->children[5];
    result->setUpdate(createAstFromParseTree(update));

    auto bodyNode = node->children[7];
    if (bodyNode->children.size() > 1) {
        auto body = bodyNode->children[0];
        result->setBody(createAstFromParseTree(body));
    }
    return result;
}

AstNode *createAstFromParseTree(ParseTreeNode *node) {
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
        return createSequence(node, nullptr);
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

    if (isForStatement(node)) {
        return createForStatement(node);
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
