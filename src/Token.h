#pragma once

#include <string>

struct Token {
    enum TokenType {
        INT_LIT,
        FLOAT_LIT,
        TRUE,
        FALSE,
        NOT,
        PLUS,
        MINUS,
        STAR,
        DIV,
        LEFT_PARAN,
        RIGHT_PARAN,
        SEMICOLON,
        END_OF_FILE
    };

    TokenType type;
    std::string content;
};

std::string to_string(Token::TokenType type);
