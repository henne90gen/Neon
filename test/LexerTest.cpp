#include "Lexer.h"
#include <catch2/catch.hpp>

TEST_CASE("Lexer can handle lots of spaces") {
    std::vector<std::string> lines = {"     1    "};
    CodeProvider *codeProvider = new StringCodeProvider(lines);
    auto lexer = Lexer(codeProvider);
    auto token = lexer.getToken();
    REQUIRE(token.content == "1");
    REQUIRE(token.type == Token::INT_LIT);
}

TEST_CASE("Lexer can handle invalid token") {
    std::vector<std::string> lines = {"invalid"};
    CodeProvider *codeProvider = new StringCodeProvider(lines);
    auto lexer = Lexer(codeProvider);
    auto token = lexer.getToken();
    REQUIRE(token.content == "invalid");
    REQUIRE(token.type == Token::END_OF_FILE);
}
