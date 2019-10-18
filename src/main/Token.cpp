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
    case Token::TRUE:
        return "TRUE";
    case Token::FALSE:
        return "FALSE";
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
    case Token::VARIABLE_NAME:
        return "VARIABLE_NAME";
    case Token::DATA_TYPE:
        return "DATA_TYPE";
    case Token::LEFT_CURLY_BRACE:
        return "LEFT_CURLY_BRACE";
    case Token::RIGHT_CURLY_BRACE:
        return "RIGHT_CURLY_BRACE";
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
    default:
        return "NOT IMPLEMENTED YET (" + std::to_string(static_cast<int>(type)) + ")";
    }
}
