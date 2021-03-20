#include "Parser.h"


AstNode *Parser::parsePrimary(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    auto beforeTokenIdx = currentTokenIdx;
    auto functionCall = parseFunctionCall(tokens, currentTokenIdx, level + 1);
    if (functionCall != nullptr) {
        return functionCall;
    }

    auto variable = parseVariable(tokens, currentTokenIdx, level + 1);
    if (variable != nullptr) {
        return variable;
    }

    auto literal = parseLiteral(tokens, currentTokenIdx, level + 1);
    if (literal != nullptr) {
        return literal;
    }

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LEFT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    auto expression = parseExpression(tokens, currentTokenIdx, level + 1);
    if (expression == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    if (!(currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::RIGHT_PARAN)) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    currentTokenIdx++;

    return expression;
}

AstNode *Parser::parseUnary(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    auto beforeTokenIdx = currentTokenIdx;
    if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::NOT) {
        currentTokenIdx++;

        auto child = parseUnary(tokens, currentTokenIdx, level + 1);
        if (child == nullptr) {
            currentTokenIdx = beforeTokenIdx;
            return nullptr;
        }

        auto node = new UnaryOperationNode(UnaryOperationNode::UnaryOperationType::NOT);
        node->setChild(child);
        return node;
    }

    return parsePrimary(tokens, currentTokenIdx, level);
}

AstNode *Parser::parseFactor(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    auto beforeTokenIdx = currentTokenIdx;
    auto lastUnary = parseUnary(tokens, currentTokenIdx, level + 1);
    if (lastUnary == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    while (true) {
        ast::BinaryOperationType operationType;
        if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::STAR) {
            operationType = ast::BinaryOperationType::MULTIPLICATION;
        } else if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::DIV) {
            operationType = ast::BinaryOperationType::DIVISION;
        } else {
            break;
        }

        currentTokenIdx++;

        auto other = parseUnary(tokens, currentTokenIdx, level + 1);
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

AstNode *Parser::parseTerm(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    auto beforeTokenIdx = currentTokenIdx;
    auto lastFactor = parseFactor(tokens, currentTokenIdx, level + 1);
    if (lastFactor == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    while (true) {
        ast::BinaryOperationType operationType;
        if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::PLUS) {
            operationType = ast::BinaryOperationType::ADDITION;
        } else if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::MINUS) {
            operationType = ast::BinaryOperationType::SUBTRACTION;
        } else {
            break;
        }

        currentTokenIdx++;

        auto other = parseFactor(tokens, currentTokenIdx, level + 1);
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

AstNode *Parser::parseComparison(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    auto beforeTokenIdx = currentTokenIdx;
    auto lastTerm = parseTerm(tokens, currentTokenIdx, level + 1);
    if (lastTerm == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    while (true) {
        ast::BinaryOperationType operationType;
        if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LESS_THAN) {
            operationType = ast::BinaryOperationType::LESS_THAN;
        } else if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::LESS_EQUALS) {
            operationType = ast::BinaryOperationType::LESS_EQUALS;
        } else if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::GREATER_THAN) {
            operationType = ast::BinaryOperationType::GREATER_THAN;
        } else if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::GREATER_EQUALS) {
            operationType = ast::BinaryOperationType::GREATER_EQUALS;
        } else {
            break;
        }

        currentTokenIdx++;

        auto other = parseTerm(tokens, currentTokenIdx, level + 1);
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

AstNode *Parser::parseEquality(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    auto beforeTokenIdx = currentTokenIdx;
    auto lastComparison = parseComparison(tokens, currentTokenIdx, level + 1);
    if (lastComparison == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }
    while (true) {
        ast::BinaryOperationType operationType;
        if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::DOUBLE_EQUALS) {
            operationType = ast::BinaryOperationType::EQUALS;
        } else if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::NOT_EQUALS) {
            operationType = ast::BinaryOperationType::NOT_EQUALS;
        } else {
            break;
        }

        currentTokenIdx++;

        auto other = parseComparison(tokens, currentTokenIdx, level + 1);
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

AstNode *Parser::parseAndOr(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    auto beforeTokenIdx = currentTokenIdx;
    auto last = parseEquality(tokens, currentTokenIdx, level + 1);
    if (last == nullptr) {
        currentTokenIdx = beforeTokenIdx;
        return nullptr;
    }

    while (true) {
        ast::BinaryOperationType operationType;
        if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::AND) {
            operationType = ast::BinaryOperationType::AND;
        } else if (currentTokenIdx < tokens.size() && tokens[currentTokenIdx].type == Token::OR) {
            operationType = ast::BinaryOperationType::OR;
        } else {
            break;
        }

        currentTokenIdx++;

        auto other = parseEquality(tokens, currentTokenIdx, level + 1);
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

AstNode *Parser::parseExpression(const std::vector<Token> &tokens, int &currentTokenIdx, int level) const {
    log.debug(indent(level) + "parsing expression node");

    return parseAndOr(tokens, currentTokenIdx, level + 1);
}
