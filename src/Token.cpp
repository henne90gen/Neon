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
        case Token::EQUALS:
            return "EQUALS";
        case Token::NOT_EQUALS:
            return "NOT_EQUALS";
        default:
            return "NOT IMPLEMENTED YET (" + std::to_string((int) type) + ")";
    }
}
