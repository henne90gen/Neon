#include "ParserTestHelper.h"
#include <catch2/catch.hpp>

TEST_CASE("Parser Operations") {
    SECTION("can handle addition") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"int a = 1 + 1"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle addition with parentheses") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"int a = (1 + 1)"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle chained addition with parentheses") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::BINARY_OPERATION},
              {5, ast::NodeType::LITERAL},          {5, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"int a = (1 + 1 + 1)"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle chained addition") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::BINARY_OPERATION},
              {5, ast::NodeType::LITERAL},          {5, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"int a = 1 + 1 + 1"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle nested addition right") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::BINARY_OPERATION}, {5, ast::NodeType::LITERAL},
              {5, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"int a = 1 + (2 + 3)"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle nested addition left") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::BINARY_OPERATION},
              {5, ast::NodeType::LITERAL},          {5, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"int b = (1 + 2) + 3"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle arithmetic operations") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},          {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},          {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},          {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"int a = 1 + 1", "int a = 1 - 1", "int a = 1 / 1", "int a = 1 * 1"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle boolean binary operations") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},          {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},          {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},          {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},          {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},          {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},          {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},          {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {
              "bool a = 1 == 0", "bool b = 1 <= 0", "bool c = 1 < 0",          "bool d = 1 >= 0",
              "bool e = 1 > 0",  "bool f = 1 != 0", "bool g = true and false", "bool g = true or false",
        };
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle boolean unary operations") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},
              {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::UNARY_OPERATION},
              {4, ast::NodeType::LITERAL},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},
              {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::UNARY_OPERATION},
              {4, ast::NodeType::BINARY_OPERATION},
              {5, ast::NodeType::LITERAL},
              {5, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {
              "bool b = not true",
              "bool b = not (true and false)",
        };
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle negate unary operations") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},
              {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::UNARY_OPERATION},
              {4, ast::NodeType::LITERAL},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},
              {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::UNARY_OPERATION},
              {4, ast::NodeType::BINARY_OPERATION},
              {5, ast::NodeType::LITERAL},
              {5, ast::NodeType::UNARY_OPERATION},
              {6, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {
              "int i = -2",
              "float f = -(1.0 + -7.0)",
        };
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }
}
