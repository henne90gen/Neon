#pragma once

#include <string>

struct Token {
    enum TokenType {
        INTEGER,
        FLOAT,
        TRUE,
        FALSE,
        AND,
        OR,
        NOT,
        PLUS,
        MINUS,
        STAR,
        DIV,
        LESS_THAN,
        GREATER_THAN,
        LESS_EQUALS,
        GREATER_EQUALS,
        EQUALS,
        NOT_EQUALS,
        LEFT_PARAN,
        RIGHT_PARAN,
        SEMICOLON,
        END_OF_FILE
    };

    TokenType type;
    std::string content;
};

std::string to_string(Token::TokenType type);
