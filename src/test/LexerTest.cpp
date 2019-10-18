#include "Lexer.h"

#include <catch2/catch.hpp>
#include <unordered_map>

TEST_CASE("Lexer can handle lots of spaces") {
    std::vector<std::string> lines = {"     1    "};
    CodeProvider *codeProvider = new StringCodeProvider(lines);
    Program program = {};
    auto lexer = Lexer(codeProvider, program);
    auto token = lexer.getToken();
    REQUIRE(token.content == "1");
    REQUIRE(token.type == Token::INTEGER);
}

TEST_CASE("Lexer can handle spaces between tokens") {
    std::vector<std::string> lines = {"1 - 5"};
    CodeProvider *codeProvider = new StringCodeProvider(lines);
    Program program = {};
    auto lexer = Lexer(codeProvider, program, false);
    auto token = lexer.getToken();
    REQUIRE(token.type == Token::INTEGER);
    REQUIRE(token.content == "1");

    token = lexer.getToken();
    REQUIRE(token.type == Token::MINUS);
    REQUIRE(token.content == "-");

    token = lexer.getToken();
    REQUIRE(token.type == Token::INTEGER);
    REQUIRE(token.content == "5");
}

TEST_CASE("Lexer can handle no spaces between tokens") {
    std::vector<std::string> lines = {"1-5"};
    CodeProvider *codeProvider = new StringCodeProvider(lines);
    Program program = {};
    auto lexer = Lexer(codeProvider, program, false);
    auto token = lexer.getToken();
    REQUIRE(token.type == Token::INTEGER);
    REQUIRE(token.content == "1");

    token = lexer.getToken();
    REQUIRE(token.type == Token::MINUS);
    REQUIRE(token.content == "-");

    token = lexer.getToken();
    REQUIRE(token.type == Token::INTEGER);
    REQUIRE(token.content == "5");
}

TEST_CASE("Lexer can handle all tokens") {
    std::unordered_map<std::string, Token::TokenType> tokens = {
          {"1", Token::INTEGER}, //
          {"1.5", Token::FLOAT},
          {"<", Token::LESS_THAN},
          {">", Token::GREATER_THAN},
          {"<=", Token::LESS_EQUALS},
          {">=", Token::GREATER_EQUALS},
          {"==", Token::DOUBLE_EQUALS},
          {"!=", Token::NOT_EQUALS},
          {"+", Token::PLUS},
          {"-", Token::MINUS},
          {"*", Token::STAR},
          {"/", Token::DIV},
          {"not", Token::NOT},
          {"and", Token::AND},
          {"or", Token::OR},
          {"=", Token::SINGLE_EQUALS},
          {"(", Token::LEFT_PARAN},
          {")", Token::RIGHT_PARAN},
          {"{", Token::LEFT_CURLY_BRACE},
          {"}", Token::RIGHT_CURLY_BRACE},
          {";", Token::SEMICOLON},
          {",", Token::COMMA},
          {"helloWorld", Token::VARIABLE_NAME},
          {"hello123World", Token::VARIABLE_NAME},
          {"_helloWorld", Token::VARIABLE_NAME},
          {"hello_World", Token::VARIABLE_NAME},
          {"int", Token::DATA_TYPE},
          {"float", Token::DATA_TYPE},
          {"bool", Token::DATA_TYPE},
          {"fun", Token::FUN},
          {"extern", Token::EXTERN},
          {"if", Token::IF},
          {"else", Token::ELSE},
          {"for", Token::FOR},
    };
    std::vector<std::string> lines;
    lines.reserve(tokens.size());
    for (auto &kv : tokens) {
        lines.push_back(kv.first);
    }

    CodeProvider *codeProvider = new StringCodeProvider(lines);
    Program program = {};
    auto lexer = Lexer(codeProvider, program, false);
    for (auto &expectedToken : tokens) {
        auto actualToken = lexer.getToken();
        INFO(to_string(actualToken.type) + " != " + to_string(expectedToken.second));
        REQUIRE(actualToken.content == expectedToken.first);
        INFO(actualToken.content + " != " + expectedToken.first);
        REQUIRE(actualToken.type == expectedToken.second);
    }
}
