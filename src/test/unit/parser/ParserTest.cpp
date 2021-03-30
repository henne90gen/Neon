#include <catch2/catch.hpp>

#include "ParserTestHelper.h"

TEST_CASE("Parser") {
    SECTION("can handle variables") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},   {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT}, {3, ast::NodeType::VARIABLE},
              {3, ast::NodeType::LITERAL},    {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT}, {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::VARIABLE},
        };
        std::vector<std::string> program = {"a = 5", "int b = a"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("can handle member access") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},      {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},    {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::MEMBER_ACCESS}, {4, ast::NodeType::VARIABLE},
              {4, ast::NodeType::VARIABLE},      {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},    {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::MEMBER_ACCESS}, {4, ast::NodeType::MEMBER_ACCESS},
              {5, ast::NodeType::VARIABLE},      {5, ast::NodeType::VARIABLE},
              {4, ast::NodeType::VARIABLE},
        };
        std::vector<std::string> program = {"int world = hello.world", "float x = hello.world.x"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }
}
