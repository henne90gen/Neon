#include "Parser.h"

MemberAccessNode *Parser::parseMemberAccess(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    auto *lastNode = AST_NODE(parseVariable(level + 1));
    if (lastNode == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    bool foundAtLeastOneDot = false;
    while (true) {
        if (!currentTokenIs(Token::DOT)) {
            break;
        }
        foundAtLeastOneDot = true;

        currentTokenIdx++;

        auto *other = parseVariable(level + 1);
        if (other == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        lastNode = AST_NODE(tree.createMemberAccess(lastNode, AST_NODE(other)));
    }

    if (!foundAtLeastOneDot) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    return &lastNode->member_access;
}

AstNode *Parser::parsePrimary(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    auto *memberAccess = parseMemberAccess(level + 1);
    if (memberAccess != nullptr) {
        return AST_NODE(memberAccess);
    }

    auto *functionCall = parseFunctionCall(level + 1);
    if (functionCall != nullptr) {
        return AST_NODE(functionCall);
    }

    auto *variable = parseVariable(level + 1);
    if (variable != nullptr) {
        return AST_NODE(variable);
    }

    auto *literal = parseLiteral(level + 1);
    if (literal != nullptr) {
        return AST_NODE(literal);
    }

    if (!currentTokenIs(Token::LEFT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto *expression = parseExpression(level + 1);
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
    ast::UnaryOperationType operationType;
    if (currentTokenIs(Token::NOT)) {
        operationType = ast::UnaryOperationType::NOT;
    } else if (currentTokenIs(Token::MINUS)) {
        operationType = ast::UnaryOperationType::NEGATE;
    } else {
        return parsePrimary(level);
    }

    currentTokenIdx++;

    auto *child = parseUnary(level + 1);
    if (child == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    return AST_NODE(tree.createUnaryOperation(operationType, child));
}

AstNode *Parser::parseFactor(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    auto *lastUnary = parseUnary(level + 1);
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

        auto *other = parseUnary(level + 1);
        if (other == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        lastUnary = AST_NODE(tree.createBinaryOperation(operationType, lastUnary, other));
    }

    return lastUnary;
}

AstNode *Parser::parseTerm(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    auto *lastFactor = parseFactor(level + 1);
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

        auto *other = parseFactor(level + 1);
        if (other == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        lastFactor = AST_NODE(tree.createBinaryOperation(operationType, lastFactor, other));
    }

    return lastFactor;
}

AstNode *Parser::parseComparison(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    auto *lastTerm = parseTerm(level + 1);
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

        auto *other = parseTerm(level + 1);
        if (other == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        lastTerm = AST_NODE(tree.createBinaryOperation(operationType, lastTerm, other));
    }

    return lastTerm;
}

AstNode *Parser::parseEquality(int level) {
    auto beforeTokenIdx = currentTokenIdx;
    auto *lastComparison = parseComparison(level + 1);
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

        auto *other = parseComparison(level + 1);
        if (other == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        lastComparison = AST_NODE(tree.createBinaryOperation(operationType, lastComparison, other));
    }

    return lastComparison;
}

AstNode *Parser::parseExpression(int level) {
    log.debug(indent(level) + "parsing expression node");
    auto beforeTokenIdx = currentTokenIdx;
    auto *lastEquality = parseEquality(level + 1);
    if (lastEquality == nullptr) {
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

        auto *other = parseEquality(level + 1);
        if (other == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        lastEquality = AST_NODE(tree.createBinaryOperation(operationType, lastEquality, other));
    }

    return lastEquality;
}
