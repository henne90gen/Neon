#include "Parser.h"

AstNode *Parser::parsePrimary(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    auto functionCall = parseFunctionCall(level + 1);
    if (functionCall != nullptr) {
        return functionCall;
    }

    auto variable = parseVariable(level + 1);
    if (variable != nullptr) {
        return variable;
    }

    auto literal = parseLiteral(level + 1);
    if (literal != nullptr) {
        return literal;
    }

    if (!currentTokenIs(Token::LEFT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto expression = parseExpression(level + 1);
    if (expression == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    if (!currentTokenIs(Token::RIGHT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    return expression;
}

AstNode *Parser::parseUnary(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    if (!currentTokenIs(Token::NOT)) {
        return parsePrimary(level);
    }

    currentTokenIdx++;

    auto child = parseUnary(level + 1);
    if (child == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    auto node = new UnaryOperationNode(UnaryOperationNode::UnaryOperationType::NOT);
    node->setChild(child);
    return node;
}

AstNode *Parser::parseFactor(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    auto lastUnary = parseUnary(level + 1);
    if (lastUnary == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    while (true) {
        ast::BinaryOperationType operationType;
        if (currentTokenIs(Token::STAR)) {
            operationType = ast::BinaryOperationType::MULTIPLICATION;
        } else if (currentTokenIs(Token::DIV)) {
            operationType = ast::BinaryOperationType::DIVISION;
        } else {
            break;
        }

        currentTokenIdx++;

        auto other = parseUnary(level + 1);
        if (other == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        auto op = new BinaryOperationNode(operationType);
        op->setLeft(lastUnary);
        op->setRight(other);
        lastUnary = op;
    }

    return lastUnary;
}

AstNode *Parser::parseTerm(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    auto lastFactor = parseFactor(level + 1);
    if (lastFactor == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    while (true) {
        ast::BinaryOperationType operationType;
        if (currentTokenIs(Token::PLUS)) {
            operationType = ast::BinaryOperationType::ADDITION;
        } else if (currentTokenIs(Token::MINUS)) {
            operationType = ast::BinaryOperationType::SUBTRACTION;
        } else {
            break;
        }

        currentTokenIdx++;

        auto other = parseFactor(level + 1);
        if (other == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        auto op = new BinaryOperationNode(operationType);
        op->setLeft(lastFactor);
        op->setRight(other);
        lastFactor = op;
    }

    return lastFactor;
}

AstNode *Parser::parseComparison(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    auto lastTerm = parseTerm(level + 1);
    if (lastTerm == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    while (true) {
        ast::BinaryOperationType operationType;
        if (currentTokenIs(Token::LESS_THAN)) {
            operationType = ast::BinaryOperationType::LESS_THAN;
        } else if (currentTokenIs(Token::LESS_EQUALS)) {
            operationType = ast::BinaryOperationType::LESS_EQUALS;
        } else if (currentTokenIs(Token::GREATER_THAN)) {
            operationType = ast::BinaryOperationType::GREATER_THAN;
        } else if (currentTokenIs(Token::GREATER_EQUALS)) {
            operationType = ast::BinaryOperationType::GREATER_EQUALS;
        } else {
            break;
        }

        currentTokenIdx++;

        auto other = parseTerm(level + 1);
        if (other == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        auto op = new BinaryOperationNode(operationType);
        op->setLeft(lastTerm);
        op->setRight(other);
        lastTerm = op;
    }

    return lastTerm;
}

AstNode *Parser::parseEquality(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    auto lastComparison = parseComparison(level + 1);
    if (lastComparison == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    while (true) {
        ast::BinaryOperationType operationType;
        if (currentTokenIs(Token::DOUBLE_EQUALS)) {
            operationType = ast::BinaryOperationType::EQUALS;
        } else if (currentTokenIs(Token::NOT_EQUALS)) {
            operationType = ast::BinaryOperationType::NOT_EQUALS;
        } else {
            break;
        }

        currentTokenIdx++;

        auto other = parseComparison(level + 1);
        if (other == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        auto op = new BinaryOperationNode(operationType);
        op->setLeft(lastComparison);
        op->setRight(other);
        lastComparison = op;
    }

    return lastComparison;
}

AstNode *Parser::parseExpression(int level) {
    log.debug(indent(level) + "parsing expression node");
    auto beforeTokenIdx = currentTokenIdx;
    auto last = parseEquality(level + 1);
    if (last == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    while (true) {
        ast::BinaryOperationType operationType;
        if (currentTokenIs(Token::AND)) {
            operationType = ast::BinaryOperationType::AND;
        } else if (currentTokenIs(Token::OR)) {
            operationType = ast::BinaryOperationType::OR;
        } else {
            break;
        }

        currentTokenIdx++;

        auto other = parseEquality(level + 1);
        if (other == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        auto op = new BinaryOperationNode(operationType);
        op->setLeft(last);
        op->setRight(other);
        last = op;
    }

    return last;
}
