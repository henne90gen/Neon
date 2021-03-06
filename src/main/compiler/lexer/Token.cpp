#include "Token.h"

#include <string>

std::string to_string(Token::TokenType type) {
    switch (type) {
    case Token::LEFT_PARAN:
        return "LEFT_PARAN";
    case Token::RIGHT_PARAN:
        return "RIGHT_PARAN";
    case Token::INTEGER:
        return "INTEGER";
    case Token::FLOAT:
        return "FLOAT";
    case Token::PLUS:
        return "PLUS";
    case Token::MINUS:
        return "MINUS";
    case Token::DIV:
        return "DIV";
    case Token::STAR:
        return "STAR";
    case Token::END_OF_FILE:
        return "END_OF_FILE";
    case Token::BOOLEAN:
        return "BOOLEAN";
    case Token::NOT:
        return "NOT";
    case Token::AND:
        return "AND";
    case Token::OR:
        return "OR";
    case Token::GREATER_THAN:
        return "GREATER_THAN";
    case Token::LESS_THAN:
        return "LESS_THAN";
    case Token::GREATER_EQUALS:
        return "GREATER_EQUALS";
    case Token::DOUBLE_EQUALS:
        return "DOUBLE_EQUALS";
    case Token::SINGLE_EQUALS:
        return "SINGLE_EQUALS";
    case Token::NOT_EQUALS:
        return "NOT_EQUALS";
    case Token::FUN:
        return "FUN";
    case Token::IDENTIFIER:
        return "IDENTIFIER";
    case Token::DOT:
        return "DOT";
    case Token::SIMPLE_DATA_TYPE:
        return "SIMPLE_DATA_TYPE";
    case Token::LEFT_CURLY_BRACE:
        return "LEFT_CURLY_BRACE";
    case Token::RIGHT_CURLY_BRACE:
        return "RIGHT_CURLY_BRACE";
    case Token::LEFT_BRACKET:
        return "LEFT_BRACKET";
    case Token::RIGHT_BRACKET:
        return "RIGHT_BRACKET";
    case Token::COMMA:
        return "COMMA";
    case Token::SEMICOLON:
        return "SEMICOLON";
    case Token::RETURN:
        return "RETURN";
    case Token::EXTERN:
        return "EXTERN";
    case Token::IF:
        return "IF";
    case Token::ELSE:
        return "ELSE";
    case Token::NEW_LINE:
        return "NEW_LINE";
    case Token::STRING:
        return "STRING";
    case Token::TYPE:
        return "TYPE";
    default:
        return "NOT IMPLEMENTED YET (" + std::to_string(static_cast<int>(type)) + ")";
    }
}
