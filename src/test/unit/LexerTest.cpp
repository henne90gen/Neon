#include <catch2/catch.hpp>

#include "compiler/lexer/Lexer.h"

#include <unordered_map>

Lexer getLexer(const std::vector<std::string> &lines, Logger &logger) {
    CodeProvider *codeProvider = new StringCodeProvider(lines, false);
    auto lexer = Lexer(codeProvider, logger);
    return lexer;
}

void assertTokensCanBeLexed(const std::unordered_map<std::string, Token::TokenType> &tokens) {
    std::vector<std::string> lines;
    lines.reserve(tokens.size());
    for (auto &kv : tokens) {
        lines.push_back(kv.first);
    }

    Logger logger = {};
    auto lexer = getLexer(lines, logger);
    for (auto &expectedToken : tokens) {
        auto actualToken = lexer.getToken();
        INFO(to_string(actualToken.type) + " != " + to_string(expectedToken.second));
        REQUIRE(actualToken.type == expectedToken.second);
        INFO(actualToken.content + " != " + expectedToken.first);
        REQUIRE(actualToken.content == expectedToken.first);
    }
}

TEST_CASE("Lexer") {
    SECTION("can handle lots of spaces") {
        std::vector<std::string> lines = {"     1    "};
        Logger logger = {};
        auto lexer = getLexer(lines, logger);
        auto token = lexer.getToken();
        REQUIRE(token.content == "1");
        REQUIRE(token.type == Token::INTEGER);
    }

    SECTION("can handle spaces between tokens") {
        std::vector<std::string> lines = {"1 - 5"};
        Logger logger = {};
        auto lexer = getLexer(lines, logger);
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

    SECTION("can handle tabs") {
        std::vector<std::string> lines = {"\t1"};
        Logger logger = {};
        auto lexer = getLexer(lines, logger);
        auto token = lexer.getToken();
        REQUIRE(token.content == "1");
        REQUIRE(token.type == Token::INTEGER);
    }

    SECTION("can handle no spaces between tokens") {
        std::vector<std::string> lines = {"1-5"};
        Logger logger = {};
        Lexer lexer = getLexer(lines, logger);
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

    SECTION("can handle all tokens") {
        std::unordered_map<std::string, Token::TokenType> tokens = {
              {"\n", Token::NEW_LINE},
              {"1", Token::INTEGER},
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
              {"[", Token::LEFT_BRACKET},
              {"]", Token::RIGHT_BRACKET},
              {";", Token::SEMICOLON},
              {",", Token::COMMA},
              {"bool", Token::SIMPLE_DATA_TYPE},
              {"int", Token::SIMPLE_DATA_TYPE},
              {"float", Token::SIMPLE_DATA_TYPE},
              {"string", Token::SIMPLE_DATA_TYPE},
              {"fun", Token::FUN},
              {"extern", Token::EXTERN},
              {"if", Token::IF},
              {"else", Token::ELSE},
              {"for", Token::FOR},
              {"import", Token::IMPORT},
              {"abc.abc", Token::MEMBER_ACCESS},
              {"abc.abc.abc", Token::MEMBER_ACCESS},
              {"abc.abc.abc.abc", Token::MEMBER_ACCESS},
        };
        assertTokensCanBeLexed(tokens);
    }

    SECTION("can handle variable names") {
        std::unordered_map<std::string, Token::TokenType> tokens = {
              {"helloWorld", Token::IDENTIFIER},
              {"hello123World", Token::IDENTIFIER},
              {"_helloWorld", Token::IDENTIFIER},
              {"hello_World", Token::IDENTIFIER},
        };
        assertTokensCanBeLexed(tokens);
    }

    SECTION("can handle strings") {
        std::unordered_map<std::string, Token::TokenType> tokens = {
              {"\"myString\"", Token::STRING},
              {"\"myString/anotherString\"", Token::STRING},
        };
        assertTokensCanBeLexed(tokens);
    }
}