#include <catch2/catch.hpp>

#include "ParserTestHelper.h"

TEST_CASE("Parser Types") {
    SECTION("can handle integers") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},   {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT}, {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"int a = 1"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle floats") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},   {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT}, {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"float a = 1.0"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle booleans") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},   {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT}, {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::LITERAL},    {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT}, {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"bool a = true", "bool b = false"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle strings") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},   {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT}, {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"string a = \"Hello World\""};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle array example") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},  {1, ast::NodeType::STATEMENT},  {2, ast::NodeType::VARIABLE_DEFINITION},
              {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::ASSIGNMENT}, {3, ast::NodeType::VARIABLE},
              {3, ast::NodeType::LITERAL},   {1, ast::NodeType::STATEMENT},  {2, ast::NodeType::ASSIGNMENT},
              {3, ast::NodeType::VARIABLE},  {3, ast::NodeType::VARIABLE},
        };
        std::vector<std::string> program = {"int[5] a", "a[0] = 5", "a[1] = a[0]"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle type declarations") {
        // TODO activate this again as soon as complex types are implemented
        return;
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::TYPE_DECLARATION},
              {3, ast::NodeType::TYPE_MEMBER},
        };
        std::vector<std::string> program = {"type MyType {", "int t", "}"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }
}
