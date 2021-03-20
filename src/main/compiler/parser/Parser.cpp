#include "Parser.h"

#include <iostream>

Token Parser::getNextToken() {
    Token token = lexer.getToken();
    module->tokens.push_back(token);
    return token;
}

bool Parser::currentTokenIs(Token::TokenType tokenType) const {
    return currentTokenIdx < module->tokens.size() && module->tokens[currentTokenIdx].type == tokenType;
}

std::string Parser::currentTokenContent() const { return module->tokens[currentTokenIdx].content; }

std::string Parser::indent(int level) {
    std::string result;
    for (int i = 0; i < level; i++) {
        result += " ";
    }
    return result;
}

ImportNode *Parser::parseImport() {
    if (!currentTokenIs(Token::IMPORT)) {
        return nullptr;
    }

    currentTokenIdx++;

    if (!currentTokenIs(Token::STRING)) {
        currentTokenIdx--;
        return nullptr;
    }

    std::string fileName = currentTokenContent();
    fileName = fileName.substr(1, fileName.size() - 2);

    auto importNode = new ImportNode();
    importNode->setFileName(fileName);

    currentTokenIdx++;

    return importNode;
}

LiteralNode *Parser::parseLiteral(int level) {
    log.debug(indent(level) + "parsing literal node");

    if (currentTokenIs(Token::INTEGER)) {
        log.debug(indent(level) + "parsing integer node");
        int value = std::stoi(currentTokenContent());
        currentTokenIdx++;
        return new IntegerNode(value);
    }

    if (currentTokenIs(Token::FLOAT)) {
        log.debug(indent(level) + "parsed float node");
        float value = std::stof(currentTokenContent());
        currentTokenIdx++;
        return new FloatNode(value);
    }

    if (currentTokenIs(Token::BOOLEAN)) {
        log.debug(indent(level) + "parsed boolean node");
        bool value = currentTokenContent() == "true";
        currentTokenIdx++;
        return new BoolNode(value);
    }

    if (currentTokenIs(Token::STRING)) {
        log.debug(indent(level) + "parsed string node");
        std::string value = currentTokenContent();
        value = value.substr(1, value.size() - 2);
        currentTokenIdx++;
        return new StringNode(value);
    }

    log.debug(indent(level) + "failed to parse literal node");
    return nullptr;
}

CallNode *Parser::parseFunctionCall(int level) {
    if (!currentTokenIs(Token::IDENTIFIER)) {
        return nullptr;
    }
    auto beforeTokenIdx = currentTokenIdx;
    std::string name = currentTokenContent();
    currentTokenIdx++;

    if (!currentTokenIs(Token::LEFT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    log.debug(indent(level) + "parsing call node");

    std::vector<AstNode *> params = {};
    while (!currentTokenIs(Token::RIGHT_PARAN)) {
        auto expression = parseExpression(level + 1);
        if (expression == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }
        params.push_back(expression);
        if (currentTokenIs(Token::COMMA)) {
            currentTokenIdx++;
        }
    }

    if (!currentTokenIs(Token::RIGHT_PARAN)) {
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

VariableNode *Parser::parseVariable(int level) {
    if (!currentTokenIs(Token::IDENTIFIER)) {
        return nullptr;
    }

    auto beforeTokenIdx = currentTokenIdx;
    std::string name = currentTokenContent();

    currentTokenIdx++;

    AstNode *expression = nullptr;
    if (currentTokenIs(Token::LEFT_BRACKET)) {
        currentTokenIdx++;

        expression = parseExpression(level + 1);
        if (expression == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }
        if (!currentTokenIs(Token::RIGHT_BRACKET)) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        currentTokenIdx++;
    }

    auto result = new VariableNode(name);
    result->setArrayIndex(expression);
    return result;
}

VariableDefinitionNode *Parser::parseVariableDefinition(int level) {
    log.debug(indent(level) + "parsing variable definition node");

    auto beforeTokenIdx = currentTokenIdx;
    if (currentTokenIs(Token::SIMPLE_DATA_TYPE)) {
        auto dataType = ast::DataType(from_string(currentTokenContent()));

        currentTokenIdx++;

        if (currentTokenIs(Token::IDENTIFIER)) {
            std::string variableName = currentTokenContent();
            auto variableDefinitionNode = new VariableDefinitionNode(variableName, dataType);
            log.debug(indent(level) + "parsed variable definition with simple data type");
            currentTokenIdx++;
            return variableDefinitionNode;
        } else if (currentTokenIs(Token::LEFT_BRACKET)) {
            currentTokenIdx++;

            auto literal = parseLiteral(level + 1);
            if (literal == nullptr || literal->getLiteralType() != LiteralNode::INTEGER) {
                currentTokenIdx = beforeTokenIdx;
                return nullptr;
            }
            auto arraySizeLiteral = reinterpret_cast<IntegerNode *>(literal);

            if (!currentTokenIs(Token::RIGHT_BRACKET)) {
                currentTokenIdx = beforeTokenIdx;
                return nullptr;
            }

            currentTokenIdx++;

            if (!currentTokenIs(Token::IDENTIFIER)) {
                currentTokenIdx = beforeTokenIdx;
                return nullptr;
            }

            std::string variableName = currentTokenContent();
            auto arraySize = arraySizeLiteral->getValue();
            auto variableDefinitionNode = new VariableDefinitionNode(variableName, dataType, arraySize);

            currentTokenIdx++;
            return variableDefinitionNode;
        }
    }

    // TODO parse custom types as well

    log.debug(indent(level) + "failed to parse variable definition");

    return nullptr;
}

SequenceNode *Parser::parseScope(int level) {
    if (!currentTokenIs(Token::LEFT_CURLY_BRACE)) {
        return nullptr;
    }
    log.debug(indent(level) + "parsing scope");

    auto beforeTokenIdx = currentTokenIdx;
    currentTokenIdx++;

    std::vector<AstNode *> children = {};
    while (true) {
        auto statement = parseStatement(level + 1);
        if (statement == nullptr) {
            break;
        }

        children.push_back(statement);
    }

    if (!currentTokenIs(Token::RIGHT_CURLY_BRACE)) {
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

FunctionNode *Parser::parseFunction(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    if (currentTokenIs(Token::EXTERN)) {
        currentTokenIdx++;
    }

    if (!currentTokenIs(Token::FUN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    if (!currentTokenIs(Token::IDENTIFIER)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    log.debug(indent(level) + "parsing function node");

    std::string functionName = currentTokenContent();
    currentTokenIdx++;

    if (!currentTokenIs(Token::LEFT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    std::vector<VariableDefinitionNode *> params = {};
    do {
        currentTokenIdx++;
        auto variableDefinitionNode = parseVariableDefinition(level + 1);
        if (variableDefinitionNode == nullptr) {
            break;
        }
        params.push_back(variableDefinitionNode);
    } while (currentTokenIs(Token::COMMA));

    if (!currentTokenIs(Token::RIGHT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    ast::DataType returnType = ast::DataType();
    if (currentTokenIs(Token::SIMPLE_DATA_TYPE)) {
        returnType = ast::DataType(from_string(currentTokenContent()));
        currentTokenIdx++;
    }

    auto body = parseScope(level + 1);

    auto functionNode = new FunctionNode(functionName, returnType);
    functionNode->setBody(body);
    for (auto node : params) {
        functionNode->getArguments().push_back(node);
    }
    return functionNode;
}

AstNode *Parser::parseAssignmentLeft(int level) {
    auto variableDefinitionNode = parseVariableDefinition(level + 1);
    if (variableDefinitionNode != nullptr) {
        return variableDefinitionNode;
    }

    auto variableNode = parseVariable(level + 1);
    if (variableNode != nullptr) {
        return variableNode;
    }

    return nullptr;
}

AssignmentNode *Parser::parseAssignment(int level) {
    log.debug(indent(level) + "parsing assignment statement");
    auto beforeTokenIdx = currentTokenIdx;
    auto left = parseAssignmentLeft(level + 1);
    if (left == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    log.debug(indent(level) + "parsed assignment left");

    if (!currentTokenIs(Token::SINGLE_EQUALS)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    currentTokenIdx++;

    auto right = parseExpression(level + 1);
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

IfStatementNode *Parser::parseIf(int level) {
    if (!currentTokenIs(Token::IF)) {
        return nullptr;
    }

    log.debug(indent(level) + "parsing if statement");
    auto beforeTokenIdx = currentTokenIdx;
    currentTokenIdx++;

    auto condition = parseExpression(level + 1);
    if (condition == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    SequenceNode *ifBody = parseScope(level + 1);
    if (ifBody == nullptr || ifBody->getChildren().empty()) {
        ifBody = nullptr;
    }

    SequenceNode *elseBody = nullptr;
    if (currentTokenIs(Token::ELSE)) {
        currentTokenIdx++;
        elseBody = parseScope(level + 1);
        if (elseBody == nullptr) {
            currentTokenIdx = beforeTokenIdx;
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

ForStatementNode *Parser::parseFor(int level) {
    if (!currentTokenIs(Token::FOR)) {
        return nullptr;
    }
    log.debug(indent(level) + "parsing for statement");

    auto beforeTokenIdx = currentTokenIdx;
    currentTokenIdx++;

    auto init = parseStatement(level + 1);
    if (init == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    if (!currentTokenIs(Token::SEMICOLON)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto condition = parseExpression(level + 1);
    if (condition == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    if (!currentTokenIs(Token::SEMICOLON)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto update = parseStatement(level + 1);
    if (update == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    auto body = parseScope(level + 1);
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

StatementNode *Parser::parseReturnStatement(int level) {
    if (!currentTokenIs(Token::RETURN)) {
        return nullptr;
    }

    log.debug(indent(level) + "parsing return statement");
    auto beforeTokenIdx = currentTokenIdx;
    currentTokenIdx++;

    auto expression = parseExpression(level + 1);
    if (expression == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    auto *statement = createStatementNode(expression);
    statement->setIsReturnStatement(true);
    return statement;
}

AssertNode *Parser::parseAssert(int level) {
    if (!currentTokenIs(Token::ASSERT)) {
        return nullptr;
    }

    log.debug(indent(level) + "parsing assert statement");

    currentTokenIdx++;

    auto expression = parseExpression(level + 1);
    if (expression == nullptr) {
        currentTokenIdx--;
        return nullptr;
    }
    auto *result = new AssertNode();
    result->setCondition(expression);
    return result;
}

CommentNode *Parser::parseComment(int level) {
    if (!currentTokenIs(Token::COMMENT)) {
        return nullptr;
    }

    log.debug("parsed comment node");

    auto result = new CommentNode(currentTokenContent());
    currentTokenIdx++;
    return result;
}

TypeMemberNode *Parser::parseMemberVariable(int level) {
    // TODO implement this
    return nullptr;
}

TypeDeclarationNode *Parser::parseTypeDeclaration(int level) {
    if (!currentTokenIs(Token::TYPE)) {
        return nullptr;
    }

    int beforeTokenIdx = currentTokenIdx;
    currentTokenIdx++;

    if (!currentTokenIs(Token::IDENTIFIER)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    std::string name = currentTokenContent();
    currentTokenIdx++;

    if (!currentTokenIs(Token::IDENTIFIER)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    std::vector<TypeMemberNode *> memberVariables = {};
    do {
        auto memberVariable = parseMemberVariable(level + 1);
        if (memberVariable == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }
        memberVariables.push_back(memberVariable);

        if (!currentTokenIs(Token::NEW_LINE)) {
            currentTokenIdx++;
        }
    } while (!currentTokenIs(Token::RIGHT_CURLY_BRACE));

    if (!currentTokenIs(Token::RIGHT_CURLY_BRACE)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto node = new TypeDeclarationNode(name);
    node->setMembers(memberVariables);
    return node;
}

StatementNode *Parser::parseStatement(int level) {
    log.debug(indent(level) + "parsing statement node");

    while (currentTokenIs(Token::NEW_LINE)) {
        currentTokenIdx++;
    }

    auto commentNode = parseComment(level + 1);
    if (commentNode != nullptr) {
        return createStatementNode(commentNode);
    }

    auto importNode = parseImport();
    if (importNode != nullptr) {
        return createStatementNode(importNode);
    }

    auto typeNode = parseTypeDeclaration(level + 1);
    if (typeNode != nullptr) {
        return createStatementNode(typeNode);
    }

    auto assertNode = parseAssert(level + 1);
    if (assertNode != nullptr) {
        return createStatementNode(assertNode);
    }

    auto callNode = parseFunctionCall(level + 1);
    if (callNode != nullptr) {
        return createStatementNode(callNode);
    }

    auto functionNode = parseFunction(level + 1);
    if (functionNode != nullptr) {
        return createStatementNode(functionNode);
    }

    auto ifNode = parseIf(level + 1);
    if (ifNode != nullptr) {
        return createStatementNode(ifNode);
    }

    auto forNode = parseFor(level + 1);
    if (forNode != nullptr) {
        return createStatementNode(forNode);
    }

    auto assignmentNode = parseAssignment(level + 1);
    if (assignmentNode != nullptr) {
        return createStatementNode(assignmentNode);
    }

    auto variableDefinition = parseVariableDefinition(level + 1);
    if (variableDefinition != nullptr) {
        return createStatementNode(variableDefinition);
    }

    auto returnStatement = parseReturnStatement(level + 1);
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

        auto statementNode = parseStatement(0);
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
