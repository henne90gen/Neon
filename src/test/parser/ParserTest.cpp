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
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle member access") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::TYPE_DECLARATION},
              {3, ast::NodeType::TYPE_MEMBER},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FUNCTION},
              {3, ast::NodeType::SEQUENCE},
              {4, ast::NodeType::STATEMENT},
              {5, ast::NodeType::ASSIGNMENT},
              {6, ast::NodeType::VARIABLE_DEFINITION},
              {6, ast::NodeType::CALL},
              {4, ast::NodeType::STATEMENT},
              {5, ast::NodeType::ASSIGNMENT},
              {6, ast::NodeType::MEMBER_ACCESS},
              {6, ast::NodeType::LITERAL},
              {4, ast::NodeType::STATEMENT},
              {5, ast::NodeType::MEMBER_ACCESS},
        };
        std::vector<std::string> program = {"type MyType {",       "int i",   "}",          "fun main() int {",
                                            "MyType t = MyType()", "t.i = 5", "return t.i", "}"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }
}
