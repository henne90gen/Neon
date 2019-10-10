#pragma once

#include <string>

struct Token {
    enum TokenType {
        INVALID,
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
        SINGLE_EQUALS,
        DOUBLE_EQUALS,
        NOT_EQUALS,
        LEFT_PARAN,
        RIGHT_PARAN,
        LEFT_CURLY_BRACE,
        RIGHT_CURLY_BRACE,
        COMMA,
        SEMICOLON,
        FUN,
        VARIABLE_NAME,
        DATA_TYPE,
        RETURN,
        EXTERN,
        END_OF_FILE,
    };

    TokenType type;
    std::string content;
};

std::string to_string(Token::TokenType type);
