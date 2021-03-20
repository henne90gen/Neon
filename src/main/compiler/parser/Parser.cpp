#include "Parser.h"

#include <iostream>

Token Parser::getNextToken() {
    Token token = lexer.getToken();
    module->tokens.push_back(token);
    return token;
}

std::string Parser::indent(int level) {
    std::string result;
    for (int i = 0; i < level; i++) {
        result += " ";
    }
    return result;
}

ImportNode *parseImport(const std::vector<Token> &tokens, int &currentTokenIdx) {
    if (tokens[currentTokenIdx].type != Token::IMPORT) {
        return nullptr;
    }

    if (currentTokenIdx + 1 >= tokens.size() || tokens[currentTokenIdx + 1].type != Token::STRING) {
        return nullptr;
    }

    std::string fileName = tokens[currentTokenIdx + 1].content;
    fileName = fileName.substr(1, fileName.size() - 2);

    auto importNode = new ImportNode();
    importNode->setFileName(fileName);

    currentTokenIdx += 2;
    return importNode;
}

LiteralNode *Parser::parseLiteral(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    log.debug(indent(level) + "parsing literal node");

    if (tokens[currentTokenIdx].type == Token::INTEGER) {
        log.debug(indent(level) + "parsing integer node");
        int value = std::stoi(tokens[currentTokenIdx].content);
        currentTokenIdx++;
        return new IntegerNode(value);
    }

    if (tokens[currentTokenIdx].type == Token::FLOAT) {
        log.debug(indent(level) + "parsed float node");
        float value = std::stof(tokens[currentTokenIdx].content);
        currentTokenIdx++;
        return new FloatNode(value);
    }

    if (tokens[currentTokenIdx].type == Token::BOOLEAN) {
        log.debug(indent(level) + "parsed boolean node");
        bool value = tokens[currentTokenIdx].content == "true";
        currentTokenIdx++;
        return new BoolNode(value);
    }

    if (tokens[currentTokenIdx].type == Token::STRING) {
        log.debug(indent(level) + "parsed string node");
        std::string value = tokens[currentTokenIdx].content;
        value = value.substr(1, value.size() - 2);
        currentTokenIdx++;
        return new StringNode(value);
    }

    log.debug(indent(level) + "failed to parse literal node");
    return nullptr;
}

CallNode *Parser::parseFunctionCall(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::IDENTIFIER)) {
        return nullptr;
    }
    int beforeTokenIdx = currentTokenIdx;
    std::string name = tokens[currentTokenIdx].content;
    currentTokenIdx++;

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LEFT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    log.debug(indent(level) + "parsing call node");

    std::vector<AstNode *> params = {};
    while (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type != Token::RIGHT_PARAN) {
        auto expression = parseExpression(tokens, currentTokenIdx, level + 1);
        if (expression == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }
        params.push_back(expression);
        if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::COMMA) {
            currentTokenIdx++;
        }
    }

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto callNode = new CallNode(name);
    for (const auto param : params) {
        callNode->getArguments().push_back(param);
    }
    return callNode;
}

VariableNode *Parser::parseVariable(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::IDENTIFIER)) {
        return nullptr;
    }
    auto beforeTokenIdx = currentTokenIdx;
    std::string name = tokens[currentTokenIdx].content;
    currentTokenIdx++;

    AstNode *expression = nullptr;
    if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LEFT_BRACKET) {
        currentTokenIdx++;

        expression = parseExpression(tokens, currentTokenIdx, level + 1);
        if (expression == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }
        if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_BRACKET)) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        currentTokenIdx++;
    }

    auto result = new VariableNode(name);
    result->setArrayIndex(expression);
    return result;
}

VariableDefinitionNode *Parser::parseVariableDefinition(const std::vector<Token> &tokens, int &currentTokenIdx,
                                                        int level) const {
    log.debug(indent(level) + "parsing variable definition node");

    auto beforeTokenIdx = currentTokenIdx;
    if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::SIMPLE_DATA_TYPE) {
        auto dataType = ast::DataType(from_string(tokens[currentTokenIdx].content));

        currentTokenIdx++;

        if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::IDENTIFIER) {
            std::string variableName = tokens[currentTokenIdx].content;
            auto variableDefinitionNode = new VariableDefinitionNode(variableName, dataType);
            log.debug(indent(level) + "parsed variable definition with simple data type");
            currentTokenIdx++;
            return variableDefinitionNode;
        } else if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LEFT_BRACKET) {
            currentTokenIdx++;

            auto literal = parseLiteral(tokens, currentTokenIdx, level + 1);
            if (literal == nullptr || literal->getLiteralType() != LiteralNode::INTEGER) {
                currentTokenIdx = beforeTokenIdx;
                return nullptr;
            }
            auto arraySizeLiteral = reinterpret_cast<IntegerNode *>(literal);

            if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_BRACKET)) {
                currentTokenIdx = beforeTokenIdx;
                return nullptr;
            }

            currentTokenIdx++;

            if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::IDENTIFIER)) {
                currentTokenIdx = beforeTokenIdx;
                return nullptr;
            }

            std::string variableName = tokens[currentTokenIdx].content;
            auto arraySize = arraySizeLiteral->getValue();
            auto variableDefinitionNode = new VariableDefinitionNode(variableName, dataType, arraySize);

            currentTokenIdx++;
            return variableDefinitionNode;
        }
    }

    // TODO parse array definitions as well
    // TODO parse custom types as well

    log.debug(indent(level) + "failed to parse variable definition");

    return nullptr;
}

SequenceNode *Parser::parseScope(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LEFT_CURLY_BRACE)) {
        return nullptr;
    }
    log.debug(indent(level) + "parsing scope");

    int beforeTokenIdx = currentTokenIdx;

    currentTokenIdx++;

    std::vector<AstNode *> children = {};
    while (true) {
        auto statement = parseStatement(tokens, currentTokenIdx, level + 1);
        if (statement == nullptr) {
            break;
        }

        children.push_back(statement);
    }

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_CURLY_BRACE)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto body = new SequenceNode();
    for (auto child : children) {
        body->getChildren().push_back(child);
    }
    return body;
}

FunctionNode *Parser::parseFunction(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    bool isExternFunc = false;
    if (tokens[currentTokenIdx].type == Token::EXTERN) {
        isExternFunc = true;
        currentTokenIdx++;
    }

    if (!(tokens[currentTokenIdx].type == Token::FUN &&
          (currentTokenIdx + 1 < tokens.size() && tokens[currentTokenIdx + 1].type == Token::IDENTIFIER))) {
        if (isExternFunc) {
            currentTokenIdx--;
        }
        return nullptr;
    }

    log.debug(indent(level) + "parsing function node");

    std::vector<VariableDefinitionNode *> params = {};
    int paramsIdx = 2;
    do {
        paramsIdx++;
        int idx = currentTokenIdx + paramsIdx;
        auto variableDefinitionNode = parseVariableDefinition(tokens, idx, level + 1);
        if (variableDefinitionNode == nullptr) {
            break;
        }
        params.push_back(variableDefinitionNode);
        paramsIdx = idx - currentTokenIdx;
    } while (currentTokenIdx + paramsIdx < tokens.size() && tokens[currentTokenIdx + paramsIdx].type == Token::COMMA);

    if (currentTokenIdx + paramsIdx < tokens.size() && tokens[currentTokenIdx + paramsIdx].type != Token::RIGHT_PARAN) {
        if (isExternFunc) {
            currentTokenIdx -= 1;
        }
        return nullptr;
    }

    std::string functionName = tokens[currentTokenIdx + 1].content;
    currentTokenIdx += paramsIdx + 1;
    ast::DataType returnType = ast::DataType();
    if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::SIMPLE_DATA_TYPE) {
        returnType = ast::DataType(from_string(tokens[currentTokenIdx].content));
        currentTokenIdx += 1;
    }

    auto body = parseScope(tokens, currentTokenIdx, level + 1);

    auto functionNode = new FunctionNode(functionName, returnType);
    functionNode->setBody(body);
    for (auto node : params) {
        functionNode->getArguments().push_back(node);
    }
    return functionNode;
}

AstNode *Parser::parseAssignmentLeft(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    auto variableDefinitionNode = parseVariableDefinition(tokens, currentTokenIdx, level + 1);
    if (variableDefinitionNode != nullptr) {
        return variableDefinitionNode;
    }

    auto variableNode = parseVariable(tokens, currentTokenIdx, level + 1);
    if (variableNode != nullptr) {
        return variableNode;
    }

    return nullptr;
}

AssignmentNode *Parser::parseAssignment(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    log.debug(indent(level) + "parsing assignment statement");
    auto beforeTokenIdx = currentTokenIdx;
    auto left = parseAssignmentLeft(tokens, currentTokenIdx, level + 1);
    if (left == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    log.debug(indent(level) + "parsed assignment left");

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::SINGLE_EQUALS)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    currentTokenIdx++;

    auto right = parseExpression(tokens, currentTokenIdx, level + 1);
    if (right == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    log.debug(indent(level) + "parsed assignment right");

    auto assignmentNode = new AssignmentNode();
    assignmentNode->setLeft(left);
    assignmentNode->setRight(right);
    return assignmentNode;
}

IfStatementNode *Parser::parseIf(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::IF)) {
        return nullptr;
    }

    log.debug(indent(level) + "parsing if statement");

    currentTokenIdx++;

    auto condition = parseExpression(tokens, currentTokenIdx, level + 1);
    if (condition == nullptr) {
        currentTokenIdx--;
        return nullptr;
    }

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LEFT_CURLY_BRACE)) {
        currentTokenIdx--;
        return nullptr;
    }

    SequenceNode *ifBody = parseScope(tokens, currentTokenIdx, level + 1);
    if (ifBody == nullptr || ifBody->getChildren().empty()) {
        ifBody = nullptr;
    }

    SequenceNode *elseBody = nullptr;
    if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::ELSE) {
        currentTokenIdx++;
        elseBody = parseScope(tokens, currentTokenIdx, level + 1);
        if (elseBody == nullptr) {
            currentTokenIdx--;
            return nullptr;
        } else if (elseBody->getChildren().empty()) {
            elseBody = nullptr;
        }
    }

    auto ifNode = new IfStatementNode();
    ifNode->setCondition(condition);
    ifNode->setIfBody(ifBody);
    ifNode->setElseBody(elseBody);
    return ifNode;
}

ForStatementNode *Parser::parseFor(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::FOR)) {
        return nullptr;
    }
    log.debug(indent(level) + "parsing for statement");

    auto beforeTokenIdx = currentTokenIdx;
    currentTokenIdx++;

    auto init = parseStatement(tokens, currentTokenIdx, level + 1);
    if (init == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::SEMICOLON)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto condition = parseExpression(tokens, currentTokenIdx, level + 1);
    if (condition == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::SEMICOLON)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto update = parseStatement(tokens, currentTokenIdx, level + 1);
    if (update == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    auto body = parseScope(tokens, currentTokenIdx, level + 1);
    if (body == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    auto forStatement = new ForStatementNode();
    forStatement->setInit(init);
    forStatement->setCondition(condition);
    forStatement->setUpdate(update);
    forStatement->setBody(body);
    return forStatement;
}

StatementNode *createStatementNode(AstNode *child) {
    auto statement = new StatementNode();
    statement->setChild(child);
    return statement;
}

StatementNode *Parser::parseReturnStatement(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RETURN)) {
        return nullptr;
    }

    log.debug(indent(level) + "parsing return statement");

    currentTokenIdx++;

    auto expression = parseExpression(tokens, currentTokenIdx, level + 1);
    if (expression == nullptr) {
        currentTokenIdx--;
        return nullptr;
    }

    auto *statement = createStatementNode(expression);
    statement->setIsReturnStatement(true);
    return statement;
}

AssertNode *Parser::parseAssert(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::ASSERT)) {
        return nullptr;
    }

    log.debug(indent(level) + "parsing assert statement");

    currentTokenIdx++;

    auto expression = parseExpression(tokens, currentTokenIdx, level + 1);
    if (expression == nullptr) {
        currentTokenIdx--;
        return nullptr;
    }
    auto *result = new AssertNode();
    result->setCondition(expression);
    return result;
}

CommentNode *Parser::parseComment(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::COMMENT)) {
        return nullptr;
    }

    log.debug("parsed comment node");

    auto result = new CommentNode(tokens[currentTokenIdx].content);
    currentTokenIdx++;
    return result;
}

TypeMemberNode *Parser::parseMemberVariable(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    // TODO implement this
    return nullptr;
}

TypeDeclarationNode *Parser::parseTypeDeclaration(const std::vector<Token> &tokens, int &currentTokenIdx,
                                                  int level) const {
    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::TYPE)) {
        return nullptr;
    }

    int beforeTokenIdx = currentTokenIdx;
    currentTokenIdx++;

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::IDENTIFIER)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    std::string name = tokens[currentTokenIdx].content;
    currentTokenIdx++;

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::IDENTIFIER)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    std::vector<TypeMemberNode *> memberVariables = {};
    do {
        auto memberVariable = parseMemberVariable(tokens, currentTokenIdx, level + 1);
        if (memberVariable == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }
        memberVariables.push_back(memberVariable);

        if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::NEW_LINE)) {
            currentTokenIdx++;
        }
    } while (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_CURLY_BRACE));

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_CURLY_BRACE)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto node = new TypeDeclarationNode(name);
    node->setMembers(memberVariables);
    return node;
}

StatementNode *Parser::parseStatement(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    log.debug(indent(level) + "parsing statement node");

    while (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::NEW_LINE) {
        currentTokenIdx++;
    }

    auto commentNode = parseComment(tokens, currentTokenIdx, level + 1);
    if (commentNode != nullptr) {
        return createStatementNode(commentNode);
    }

    auto importNode = parseImport(tokens, currentTokenIdx);
    if (importNode != nullptr) {
        return createStatementNode(importNode);
    }

    auto typeNode = parseTypeDeclaration(tokens, currentTokenIdx, level + 1);
    if (typeNode != nullptr) {
        return createStatementNode(typeNode);
    }

    auto assertNode = parseAssert(tokens, currentTokenIdx, level + 1);
    if (assertNode != nullptr) {
        return createStatementNode(assertNode);
    }

    auto callNode = parseFunctionCall(tokens, currentTokenIdx, level + 1);
    if (callNode != nullptr) {
        return createStatementNode(callNode);
    }

    auto functionNode = parseFunction(tokens, currentTokenIdx, level + 1);
    if (functionNode != nullptr) {
        return createStatementNode(functionNode);
    }

    auto ifNode = parseIf(tokens, currentTokenIdx, level + 1);
    if (ifNode != nullptr) {
        return createStatementNode(ifNode);
    }

    auto forNode = parseFor(tokens, currentTokenIdx, level + 1);
    if (forNode != nullptr) {
        return createStatementNode(forNode);
    }

    auto assignmentNode = parseAssignment(tokens, currentTokenIdx, level + 1);
    if (assignmentNode != nullptr) {
        return createStatementNode(assignmentNode);
    }

    auto variableDefinition = parseVariableDefinition(tokens, currentTokenIdx, level + 1);
    if (variableDefinition != nullptr) {
        return createStatementNode(variableDefinition);
    }

    auto returnStatement = parseReturnStatement(tokens, currentTokenIdx, level + 1);
    if (returnStatement != nullptr) {
        return returnStatement;
    }

    return nullptr;
}

void Parser::run() {
    while (true) {
        const Token token = getNextToken();
        if (token.type == Token::INVALID) {
            break;
        }
    }

    int currentTokenIdx = 0;
    std::vector<AstNode *> children = {};
    bool error = false;

    int count = 0;
    while (currentTokenIdx < module->tokens.size()) {
        auto token = module->tokens[currentTokenIdx];
        if (token.type == Token::INVALID) {
            error = currentTokenIdx != module->tokens.size() - 1;
            break;
        }

        if (token.type == Token::NEW_LINE) {
            currentTokenIdx++;
            continue;
        }

        auto statementNode = parseStatement(module->tokens, currentTokenIdx, 0);
        if (statementNode != nullptr) {
            children.push_back(statementNode);
            continue;
        }

        log.error("Unexpected token: " + to_string(token.type) + ": " + token.content);

        if (count > 2) {
            error = true;
            break;
        }
        count++;
    }

    if (error) {
        for (auto child : children) {
            delete child;
        }
        return;
    }

    auto sequence = new SequenceNode();
    for (auto child : children) {
        sequence->getChildren().push_back(child);
    }
    module->root = sequence;
}
