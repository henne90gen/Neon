#include "ParserTest.h"

#include <catch2/catch.hpp>

TEST_CASE("Parser Operations") {
    SECTION("can handle 'int i = 1 + 1.5'") {
        std::vector<std::string> program = {"int i = 1 + 1.5"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'int i = 4 - 5'") {
        std::vector<std::string> program = {"int i = 4 - 5"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'bool b = not true'") {
        std::vector<std::string> program = {"bool b = not true"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'bool b = 7 < 8'") {
        std::vector<std::string> program = {"bool b = 7 < 8"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'bool b = not (1 < 2)'") {
        std::vector<std::string> program = {"bool b = not (1 < 2)"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'bool b = true and false'") {
        std::vector<std::string> program = {"bool b = true and false"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'a = 5'") {
        std::vector<std::string> program = {"a = 5"};
        assertParserAccepts(program);
    }
}
