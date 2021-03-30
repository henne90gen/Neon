#include "ParserTestHelper.h"
#include <catch2/catch.hpp>

TEST_CASE("Parser Functions") {
    SECTION("function definition") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FUNCTION},
              {3, ast::NodeType::SEQUENCE},
        };
        std::vector<std::string> program = {"fun hello() { }"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("function definition with body") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FUNCTION},
              {3, ast::NodeType::SEQUENCE},
              {4, ast::NodeType::STATEMENT},
              {5, ast::NodeType::ASSIGNMENT},
              {6, ast::NodeType::VARIABLE_DEFINITION},
              {6, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"fun hello() {", "int a = 1", "}"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("main function definition") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE}, {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::FUNCTION},
              {3, ast::NodeType::SEQUENCE}, {4, ast::NodeType::STATEMENT}, {5, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"fun main() {", "return 0", "}"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("function call") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::CALL},
        };
        std::vector<std::string> program = {"hello()"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("function call with literal argument") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE}, //
              {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::CALL}, {3, ast::NodeType::LITERAL},
              {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::CALL}, {3, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {
              "hello(1)",
              "return hello(1)",
        };
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("function call with variable argument") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE}, //
              {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::CALL}, {3, ast::NodeType::VARIABLE},
              {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::CALL}, {3, ast::NodeType::VARIABLE},
        };
        std::vector<std::string> program = {
              "hello(num)",
              "return hello(num)",
        };
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("function call with member access argument") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE}, //
              {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::CALL},          {3, ast::NodeType::MEMBER_ACCESS},
              {4, ast::NodeType::VARIABLE},  {4, ast::NodeType::VARIABLE},      {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::CALL},      {3, ast::NodeType::MEMBER_ACCESS}, {4, ast::NodeType::VARIABLE},
              {4, ast::NodeType::VARIABLE},
        };
        std::vector<std::string> program = {
              "hello(num.x)",
              "return hello(num.x)",
        };
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("function call with expression argument") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE}, //
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::CALL},
              {3, ast::NodeType::BINARY_OPERATION},
              {4, ast::NodeType::VARIABLE},
              {4, ast::NodeType::LITERAL},

              // return tests
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::CALL},
              {3, ast::NodeType::BINARY_OPERATION},
              {4, ast::NodeType::VARIABLE},
              {4, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {
              "hello(num - 1)",
              "return hello(num - 1)",
        };
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("function call with multiple arguments") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},  {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::CALL},
              {3, ast::NodeType::LITERAL},   {3, ast::NodeType::LITERAL},   {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::CALL},      {3, ast::NodeType::VARIABLE},  {3, ast::NodeType::LITERAL},
              {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::CALL},      {3, ast::NodeType::VARIABLE},
              {3, ast::NodeType::VARIABLE},  {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::CALL},
              {3, ast::NodeType::VARIABLE},  {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::CALL},
              {3, ast::NodeType::VARIABLE},  {3, ast::NodeType::VARIABLE},
        };
        std::vector<std::string> program = {
              "hello(1, 2)", "hello(num, 2)", "hello(num, num)", "hello(num, )", "hello(num, num)",
        };
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("external function") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FUNCTION},
        };
        std::vector<std::string> program = {"extern fun hello()"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("external function with return type") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},  {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::FUNCTION},
              {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::FUNCTION},  {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FUNCTION},
        };
        std::vector<std::string> program = {
              "extern fun hello() int",
              "extern fun hello() float",
              "extern fun hello() bool",
        };
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("external function with arguments") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FUNCTION},
              {3, ast::NodeType::VARIABLE_DEFINITION},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FUNCTION},
              {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::VARIABLE_DEFINITION},
        };
        std::vector<std::string> program = {"extern fun hello(int i)", "extern fun hello(int i, float j)"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }

    SECTION("external function with arguments and return type") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FUNCTION},
              {3, ast::NodeType::VARIABLE_DEFINITION},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FUNCTION},
              {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::VARIABLE_DEFINITION},
        };
        std::vector<std::string> program = {"extern fun hello(int i) float", "extern fun hello(int i, float j) bool"};
        REQUIRE(parserCreatesCorrectAst(program, spec));
    }
}
