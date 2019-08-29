#pragma once

#include <string>

struct Token {
  enum TokenType {
    INT_LIT,
    FLOAT_LIT,
    //   IDENTIFIER,
    PLUS,
    MINUS,
    STAR,
    DIV,
    LEFT_PARAN,
    RIGHT_PARAN,
    END_OF_FILE
  };

  TokenType type;
  std::string content;
};

std::string to_string(Token::TokenType type);
