#include "ASTGenerator.h"

#include <iostream>

bool isBinaryOperation(ParseTreeNode *node) {
    return (node->symbol == GrammarSymbol::SUM || node->symbol == GrammarSymbol::TERM) && node->children.size() == 3;
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

bool isSequence(ParseTreeNode *node) { return node->symbol == GrammarSymbol::STMTS && node->children.size() > 1; }

bool isStatement(ParseTreeNode *node) { return node->symbol == GrammarSymbol::STMT; }

bool isFunction(ParseTreeNode *node) { return node->symbol == GrammarSymbol::FUNCTION; }

bool isAssignment(ParseTreeNode *node) { return node->symbol == GrammarSymbol::ASSIGNMENT; }

bool isIgnored(ParseTreeNode *node) {
    return node->symbol == GrammarSymbol::SEMICOLON || node->symbol == GrammarSymbol::ENDOFFILE;
}

BinaryOperationNode::BinaryOperationType getBinaryOperationType(GrammarSymbol symbol) {
    switch (symbol) {
    case GrammarSymbol::PLUS:
        return BinaryOperationNode::ADDITION;
    case GrammarSymbol::MINUS:
        return BinaryOperationNode::SUBTRACTION;
    case GrammarSymbol::STAR:
        return BinaryOperationNode::MULTIPLICATION;
    case GrammarSymbol::DIV:
        return BinaryOperationNode::DIVISION;
    default:
        std::cout << "Could not determine binary opartor type! (" << to_string(symbol) << ")" << std::endl;
        exit(1);
        return BinaryOperationNode::ADDITION;
    }
}

UnaryOperationNode::UnaryOperationType getUnaryOperationType(GrammarSymbol symbol) {
    switch (symbol) {
    case GrammarSymbol::NOT:
        return UnaryOperationNode::NOT;
    default:
        std::cout << "Could not determine unary opartor type! (" << to_string(symbol) << ")" << std::endl;
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

AstNode::DataType getDataType(ParseTreeNode *node) {
    if (node->token.type != Token::DATA_TYPE) {
        return AstNode::DataType::VOID;
    }

    if (node->token.content == "int") {
        return AstNode::DataType::INT;
    } else if (node->token.content == "float") {
        return AstNode::DataType::FLOAT;
    } else if (node->token.content == "bool") {
        return AstNode::DataType::BOOL;
    }
    return AstNode::DataType::VOID;
}

VariableDefinitionNode *createVariableDefinition(ParseTreeNode *node) {
    auto dataTypeNode = node->children[0];
    auto nameNode = node->children[1];
    AstNode::DataType dataType = getDataType(dataTypeNode);
    return new VariableDefinitionNode(nameNode->token.content, dataType);
}

AstNode *createSequence(ParseTreeNode *node, SequenceNode *seqRoot = nullptr) {
    if (seqRoot == nullptr) {
        seqRoot = new SequenceNode();
    }

    for (auto child : node->children) {
        if (isSequence(child)) {
            seqRoot = (SequenceNode *)createSequence(child, seqRoot);
            continue;
        }
        auto astChild = createAstFromParseTree(child);
        if (astChild != nullptr) {
            seqRoot->getChildren().push_back(astChild);
        }
    }

    if (seqRoot->getChildren().size() == 1) {
        auto result = seqRoot->getChildren()[0];
        delete seqRoot;
        return result;
    }

    return seqRoot;
}

AstNode *createStatement(ParseTreeNode *node) {
    auto statementNode = new StatementNode();
    if (node->children.empty()) {
        std::cout << "Statement did not contain anything." << std::endl;
        return statementNode;
    }

    bool isReturnStatement = node->children[0]->symbol == GrammarSymbol::RETURN;
    statementNode->setIsReturnStatement(isReturnStatement);

    if (node->children.size() > 1 && !isReturnStatement) {
        std::cout << "A statement should never have more than one child." << std::endl;
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

AstNode *createExternalFunction(ParseTreeNode *node) {
    return nullptr;
}

AstNode *createFunction(ParseTreeNode *node) {
    if (node->children[0]->symbol == GrammarSymbol::EXTERN) {
        return createExternalFunction(node);
    }

    //    node->children[0] = FUN
    //    node->children[1] = VARIABLE_NAME
    //    node->children[2] = LEFT_PARAN
    //    node->children[3] = FUNCTION_ARGS
    //    node->children[4] = RIGHT_PARAN
    //    node->children[5] = DATA_TYPE
    //    node->children[6] = LEFT_CURLY_BRACE
    //    node->children[7] = BODY?
    //    node->children[8] = RIGHT_CURLY_BRACE
    if (node->children.size() < 8) {
        std::cerr << "Malformed function node." << std::endl;
        return nullptr;
    }
    auto variableNameNode = node->children[1];
    auto returnType = getDataType(node->children[5]);
    auto function = new FunctionNode(variableNameNode->token.content, returnType);
    addArguments(function, node->children[3]);

    if (node->children[7]->symbol == GrammarSymbol::RIGHT_CURLY_BRACE) {
        function->setBody(new SequenceNode());
    } else {
        auto body = createAstFromParseTree(node->children[7]);
        function->setBody(body);
    }

    return function;
}

AstNode *createAssignment(ParseTreeNode *node) {
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

    if (isAssignment(node)) {
        return createAssignment(node);
    }

    if (node->children.size() == 1 || node->symbol == GrammarSymbol::PROGRAM) {
        return createAstFromParseTree(node->children[0]);
    }

    if (node->symbol == GrammarSymbol::FACTOR && node->children.size() == 3 &&
        node->children[0]->symbol == GrammarSymbol::LEFT_PARAN &&
        node->children[2]->symbol == GrammarSymbol::RIGHT_PARAN) {
        return createAstFromParseTree(node->children[1]);
    }

    if (isIgnored(node)) {
        return nullptr;
    }

    std::cout << "Could not find a suitable AstNode for " << to_string(node->symbol) << std::endl;

    return nullptr;
}
