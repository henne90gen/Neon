#include "Token.h"

#include <string>

std::string to_string(Token::TokenType type) {
  switch (type) {
  case Token::LEFT_PARAN:
    return "LEFT_PARAN";
  case Token::RIGHT_PARAN:
    return "RIGHT_PARAN";
  case Token::INT_LIT:
    return "INT_LIT";
  case Token::FLOAT_LIT:
    return "FLOAT_LIT";
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
  default:
    return "ERROR";
  }
}
