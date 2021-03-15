#include <catch2/catch.hpp>

#include "ParserTest.h"

void assertParserAccepts(const std::vector<std::string> &program) {
//    CodeProvider *cp = new StringCodeProvider(program, true);
//    auto tokens = std::vector<Token>();
//    Lexer lexer(cp, false);
//    Parser parser(lexer, tokens, false);
//
//    ParseTreeNode *actual = parser.createParseTree();
//    REQUIRE(actual != nullptr);
      REQUIRE(false);
}

TEST_CASE("Parser Basics") {
    SECTION("can handle two lines") {
        std::vector<std::string> program = {"int a = 1 + 1", "int b = a + 2"};
        assertParserAccepts(program);
    }

    SECTION("can handle return statement with function call") {
        std::vector<std::string> program = {"return hello()"};
        assertParserAccepts(program);
    }

    SECTION("can handle return statements with function calls and variable arguments") {
        std::vector<std::string> program = {"return hello(world - 1)"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'if true { } else { }'") {
        std::vector<std::string> program = {"if true { } else { }"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'if true { }'") {
        std::vector<std::string> program = {"if true { }"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'for int i = 0 ; i < 10 ; i = i + 1 { }'") {
        std::vector<std::string> program = {"for int i = 0; i< 10; i = i + 1 { }"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'int[5] a \n a[0] = 5 \n a[1] = a[0]'") {
        std::vector<std::string> program = {"int[5] a", "a[0] = 5", "a[1] = a[0]"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'import \"test\"") {
        std::vector<std::string> program = {"import \"test\""};
        assertParserAccepts(program);
    }

    SECTION("can handle 'string s = \"Hello World!\"'") {
        std::vector<std::string> program = {"string s = \"Hello World!\""};
        assertParserAccepts(program);
    }

    SECTION("can handle complex types") {
        std::vector<std::string> program = {"type MyType {",       "int t",   "}",          "fun main() int {",
                                            "MyType t = MyType()", "t.i = 5", "return t.i", "}"};
        assertParserAccepts(program);
    }
}
