#include "Parser.h"

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

    auto statement = new StatementNode();
    statement->setChild(expression);
    statement->setIsReturnStatement(true);
    return statement;
}
