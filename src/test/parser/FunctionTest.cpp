#include "ParserTest.h"

#include <catch2/catch.hpp>

TEST_CASE("Parser Functions") {
    SECTION("function definition") {
        std::vector<std::string> program = {"fun helloWorld() { }"};
        assertParserAccepts(program);
    }

    SECTION("function definition with argument and return type") {
        std::vector<std::string> program = {"fun hello(int i) float { }"};
        assertParserAccepts(program);
    }

    SECTION("function definition with multiple arguments and return type") {
        std::vector<std::string> program = {"fun hello(int i, bool b) float { }"};
        assertParserAccepts(program);
    }

    SECTION("external function definition") {
        std::vector<std::string> program = {"extern fun calc()"};
        assertParserAccepts(program);
    }

    SECTION("external function definition with return type") {
        std::vector<std::string> program = {"extern fun calc() int"};
        assertParserAccepts(program);
    }

    SECTION("external function definition with arguments and return type") {
        std::vector<std::string> program = {"extern fun calc(int i) int"};
        assertParserAccepts(program);
    }

    SECTION("function call without arguments") {
        std::vector<std::string> program = {"hello()"};
        assertParserAccepts(program);
    }
}
