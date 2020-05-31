#include "AstGenerator.h"

#include <iostream>

bool isBinaryOperation(ParseTreeNode *node) {
    return (node->symbol == GrammarSymbol::SUM || node->symbol == GrammarSymbol::TERM ||
            node->symbol == GrammarSymbol::RELATION) &&
           node->children.size() == 3;
}

bool isUnaryOperation(ParseTreeNode *node) {
    return (node->symbol == GrammarSymbol::NEGATION) && node->children.size() == 2;
}

bool isLiteral(ParseTreeNode *node) {
    return node->symbol == GrammarSymbol::BOOLEAN || node->symbol == GrammarSymbol::INTEGER ||
           node->symbol == GrammarSymbol::FLOAT || node->symbol == GrammarSymbol::STRING;
}

bool isVariable(ParseTreeNode *node) {
    return node->symbol == GrammarSymbol::IDENTIFIER || node->symbol == GrammarSymbol::ARRAY_ACCESS;
}

bool isVariableDefinition(ParseTreeNode *node) { return node->symbol == GrammarSymbol::DEFINITION; }

bool isSequence(ParseTreeNode *node) { return node->symbol == GrammarSymbol::STATEMENTS; }

bool isStatement(ParseTreeNode *node) { return node->symbol == GrammarSymbol::STATEMENT; }

bool isFunction(ParseTreeNode *node) { return node->symbol == GrammarSymbol::FUNCTION; }

bool isExternalFunction(ParseTreeNode *node) { return node->symbol == GrammarSymbol::EXTERNAL_FUNCTION; }

bool isIfStatement(ParseTreeNode *node) { return node->symbol == GrammarSymbol::IF_STATEMENT; }

bool isCall(ParseTreeNode *node) { return node->symbol == GrammarSymbol::CALL; }

bool isAssignment(ParseTreeNode *node) { return node->symbol == GrammarSymbol::ASSIGNMENT; }

bool isForStatement(ParseTreeNode *node) { return node->symbol == GrammarSymbol::FOR_STATEMENT; }

bool isImportStatement(ParseTreeNode *node) { return node->symbol == GrammarSymbol::IMPORT_STATEMENT; }

bool isTypeDeclaration(ParseTreeNode *node) { return node->symbol == GrammarSymbol::TYPE_DECLARATION; }

bool isMemberAccess(ParseTreeNode *node) { return node->symbol == GrammarSymbol::MEMBER_ACCESS; }

bool isIgnored(ParseTreeNode *node) {
    return node->symbol == GrammarSymbol::SEMICOLON || node->symbol == GrammarSymbol::NEW_LINE ||
           node->symbol == GrammarSymbol::ENDOFFILE;
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

AstNode *AstGenerator::createBinaryOperation(ParseTreeNode *node) {
    auto nodeType = getBinaryOperationType(node->children[1]->symbol);
    auto astNode = new BinaryOperationNode(nodeType);

    auto leftNode = createAstFromParseTree(node->children[0]);
    astNode->setLeft(leftNode);

    auto rightNode = createAstFromParseTree(node->children[2]);
    astNode->setRight(rightNode);
    return astNode;
}

AstNode *AstGenerator::createUnaryOperation(ParseTreeNode *node) {
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
    case GrammarSymbol::BOOLEAN: {
        bool value = false;
        if (node->token.content == "true") {
            value = true;
        }
        return new BoolNode(value);
    }
    case GrammarSymbol::STRING: {
        std::string value = node->token.content;
        value = value.substr(1, value.size() - 2);
        return new StringNode(value);
    }
    default:
        std::cout << "Data type not supported yet!" << std::endl;
        return nullptr;
    }
}

VariableNode *AstGenerator::createVariable(ParseTreeNode *node) {
    if (node->children.empty()) {
        return new VariableNode(node->token.content);
    }

    auto result = new VariableNode(node->children[0]->token.content);
    result->setArrayIndex(createAstFromParseTree(node->children[2]));
    return result;
}

ast::DataType getDataType(ParseTreeNode *node) {
    if (node->token.type != Token::SIMPLE_DATA_TYPE) {
        return ast::DataType(node->token.content);
    }

    if (node->token.content == "bool") {
        return ast::DataType(ast::SimpleDataType::BOOL);
    }
    if (node->token.content == "int") {
        return ast::DataType(ast::SimpleDataType::INT);
    }
    if (node->token.content == "float") {
        return ast::DataType(ast::SimpleDataType::FLOAT);
    }
    if (node->token.content == "string") {
        return ast::DataType(ast::SimpleDataType::STRING);
    }
    return ast::DataType(ast::SimpleDataType::VOID);
}

VariableDefinitionNode *AstGenerator::createVariableDefinition(ParseTreeNode *node) {
    if (node == nullptr) {
        std::cerr << "This was predicted by clang-analyzer, but had not been verified yet." << std::endl;
        return nullptr;
    }
    if (node->children.size() == 1) {
        return createVariableDefinition(node->children[0]);
    }

    auto dataTypeNode = node->children[0];
    ast::DataType dataType = getDataType(dataTypeNode);
    int variableNameIndex = 1;
    unsigned int arraySize = 0;
    if (node->symbol == GrammarSymbol::ARRAY_DEFINITION) {
        variableNameIndex = 4;
        if (node->children[2]->symbol != GrammarSymbol::INTEGER) {
            std::cout << "Array size has to be an integer" << std::endl;
            return nullptr;
        }
        arraySize = std::stoi(node->children[2]->token.content);
    }
    auto nameNode = node->children[variableNameIndex];
    return new VariableDefinitionNode(nameNode->token.content, dataType, arraySize);
}

AstNode *AstGenerator::createSequence(ParseTreeNode *node, SequenceNode *seqRoot) {
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

StatementNode *AstGenerator::createStatement(ParseTreeNode *node) {
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
    if (child == nullptr) {
        return nullptr;
    }
    statementNode->setChild(child);

    return statementNode;
}

void AstGenerator::addArguments(FunctionNode *function, ParseTreeNode *root) {
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

FunctionNode *AstGenerator::createExternalFunction(ParseTreeNode *node) {
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

    auto returnType = ast::DataType(ast::SimpleDataType::VOID);
    if (returnTypeNode != nullptr) {
        returnType = getDataType(returnTypeNode);
    }

    auto function = new FunctionNode(variableNameNode->token.content, returnType);
    if (argumentsNode != nullptr) {
        addArguments(function, argumentsNode);
    }
    return function;
}

FunctionNode *AstGenerator::createFunction(ParseTreeNode *node) {
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

    auto returnType = ast::DataType(ast::SimpleDataType::VOID);
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

AssignmentNode *AstGenerator::createAssignment(ParseTreeNode *node) {
    AstNode *left = nullptr;
    AstNode *right = nullptr;
    auto assignmentLeft = node->children[0];
    if (assignmentLeft->children.size() == 1) {
        left = createAstFromParseTree(assignmentLeft->children[0]);
        right = createAstFromParseTree(node->children[2]);
    } else if (assignmentLeft->children.size() == 4) {
        left = new VariableNode(assignmentLeft->children[0]->token.content);
        auto arrayIndex = createAstFromParseTree(assignmentLeft->children[2]);
        auto leftVariableNode = dynamic_cast<VariableNode *>(left);
        leftVariableNode->setArrayIndex(arrayIndex);
        right = createAstFromParseTree(node->children[2]);
    } else {
        std::cout << "Invalid assignment node" << std::endl;
        return nullptr;
    }
    auto assignment = new AssignmentNode();
    assignment->setLeft(left);
    assignment->setRight(right);
    return assignment;
}

void AstGenerator::addArguments(CallNode *call, ParseTreeNode *root) {
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

CallNode *AstGenerator::createCall(ParseTreeNode *node) {
    std::string name = node->children[0]->token.content;
    auto call = new CallNode(name);
    auto call_header = node->children[2];
    if (call_header->children.size() == 2) {
        addArguments(call, call_header->children[0]);
    }
    return call;
}

IfStatementNode *AstGenerator::createIfStatement(ParseTreeNode *node) {
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

ForStatementNode *AstGenerator::createForStatement(ParseTreeNode *node) {
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

AstNode *AstGenerator::createImportStatement(ParseTreeNode *node) {
    auto result = new ImportNode();
    // TODO at what point should we remove '"' from the string?
    std::string fileName = node->children[1]->token.content;
    fileName = fileName.substr(1, fileName.size() - 2);
    result->setFileName(fileName);
    return result;
}

TypeMemberNode *createTypeMember(ParseTreeNode *node) {
    ParseTreeNode *definition = node->children[0];
    ParseTreeNode *variableDefinition = definition->children[0];
    ParseTreeNode *dataTypeNode = variableDefinition->children[0];
    ast::DataType dataType = getDataType(dataTypeNode);
    ParseTreeNode *identifier = variableDefinition->children[1];
    std::string name = identifier->token.content;
    return new TypeMemberNode(name, dataType);
}

AstNode *AstGenerator::createTypeDeclaration(ParseTreeNode *node) {
    std::string name = node->children[1]->token.content;
    auto result = new TypeDeclarationNode(name);

    auto body = node->children[4];
    if (body->children.size() < 2) {
        return result;
    }

    auto members = std::vector<TypeMemberNode *>();
    auto currentNode = body->children[0];
    while (currentNode) {
        unsigned long childrenSize = currentNode->children.size();
        if (childrenSize == 1) {
            auto member = createTypeMember(currentNode->children[0]);
            members.push_back(member);
            break;
        } else if (childrenSize == 2) {
            auto member = createTypeMember(currentNode->children[1]);
            members.push_back(member);
            currentNode = currentNode->children[0];
        } else {
            // FIXME log error
        }
    }

    // we collected the members in reverse order
    std::reverse(members.begin(), members.end());
    result->setMembers(members);

    return result;
}

AstNode *AstGenerator::createMemberAccess(ParseTreeNode *node) {
    std::string currentStr;
    std::vector<std::string> parts = {};
    for (char c : node->token.content) {
        if (c == '.') {
            parts.push_back(currentStr);
            currentStr = "";
        } else {
            currentStr += c;
        }
    }
    parts.push_back(currentStr);
    return new MemberAccessNode(parts);
}

AstNode *AstGenerator::createAstFromParseTree(ParseTreeNode *node) {
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

    if (isImportStatement(node)) {
        return createImportStatement(node);
    }

    if (isTypeDeclaration(node)) {
        return createTypeDeclaration(node);
    }

    if (isMemberAccess(node)) {
        return createMemberAccess(node);
    }

    if (node->children.size() == 1 || node->symbol == GrammarSymbol::PROGRAM) {
        // ignores nodes that have only one child
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

void AstGenerator::run(ParseTreeNode *root) { module->root = createAstFromParseTree(root); }
