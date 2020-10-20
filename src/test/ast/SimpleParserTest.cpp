#include <catch2/catch.hpp>

#include "AstTestHelper.h"

TEST_CASE("SimpleParser") {
    SECTION("can handle addition") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"int a = 1 + 1"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle addition with parentheses") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"int a = (1 + 1)"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle chained addition with parentheses") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::BINARY_OPERATION}, {5, ast::NodeType::LITERAL},
              {5, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"int a = (1 + 1 + 1)"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle chained addition") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT},       {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::LITERAL},
              {4, ast::NodeType::BINARY_OPERATION}, {5, ast::NodeType::LITERAL},
              {5, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"int a = 1 + 1 + 1"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
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
        assertProgramCreatesAstWithSimpleParser(program, spec);
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
        assertProgramCreatesAstWithSimpleParser(program, spec);
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
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle 'fun hello() { }'") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FUNCTION},
              {3, ast::NodeType::SEQUENCE},
        };
        std::vector<std::string> program = {"fun hello() { }"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle 'fun hello() { int a = 1 }'") {
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
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle function call") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::CALL},
        };
        std::vector<std::string> program = {"hello()"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle function call with argument") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::CALL},
              {3, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"hello(1)"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle external function") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FUNCTION},
        };
        std::vector<std::string> program = {"extern fun hello()"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle external function with return type") {
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
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle external function with argument") {
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
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle 'if true { }'") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::IF_STATEMENT},
              {3, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"if true { }"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle 'if true { } else { }'") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::IF_STATEMENT},
              {3, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"if true { } else { }"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle relational operations") {
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
              {4, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"bool a = 1 == 0", "bool b = 1 <= 0", "bool c = 1 < 0",
                                            "bool d = 1 >= 0", "bool e = 1 > 0",  "bool f = 1 != 0"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle 'for int i = 0; i < 10; i = i + 1 { }'") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FOR_STATEMENT},
              {3, ast::NodeType::ASSIGNMENT},
              {4, ast::NodeType::VARIABLE_DEFINITION},
              {4, ast::NodeType::LITERAL},
              {3, ast::NodeType::BINARY_OPERATION},
              {4, ast::NodeType::VARIABLE},
              {4, ast::NodeType::LITERAL},
              {3, ast::NodeType::ASSIGNMENT},
              {4, ast::NodeType::VARIABLE},
              {4, ast::NodeType::BINARY_OPERATION},
              {5, ast::NodeType::VARIABLE},
              {5, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"for int i = 0; i < 10; i = i + 1 { }"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle 'for int i = 0; i < 10; i = i + 1 { \n int a = 0 \n }'") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FOR_STATEMENT},
              {3, ast::NodeType::ASSIGNMENT},
              {4, ast::NodeType::VARIABLE_DEFINITION},
              {4, ast::NodeType::LITERAL},
              {3, ast::NodeType::BINARY_OPERATION},
              {4, ast::NodeType::VARIABLE},
              {4, ast::NodeType::LITERAL},
              {3, ast::NodeType::ASSIGNMENT},
              {4, ast::NodeType::VARIABLE},
              {4, ast::NodeType::BINARY_OPERATION},
              {5, ast::NodeType::VARIABLE},
              {5, ast::NodeType::LITERAL},
              {3, ast::NodeType::SEQUENCE},
              {4, ast::NodeType::STATEMENT},
              {5, ast::NodeType::ASSIGNMENT},
              {6, ast::NodeType::VARIABLE_DEFINITION},
              {6, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"for int i = 0; i < 10; i = i + 1 {", "int a = 0", "}"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle array example") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},  {1, ast::NodeType::STATEMENT},  {2, ast::NodeType::VARIABLE_DEFINITION},
              {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::ASSIGNMENT}, {3, ast::NodeType::VARIABLE},
              {3, ast::NodeType::LITERAL},   {1, ast::NodeType::STATEMENT},  {2, ast::NodeType::ASSIGNMENT},
              {3, ast::NodeType::VARIABLE},  {3, ast::NodeType::VARIABLE},
        };
        std::vector<std::string> program = {"int[5] a", "a[0] = 5", "a[1] = a[0]"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle import statement") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::IMPORT},
        };
        std::vector<std::string> program = {"import \"examples/functions\""};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle string definition") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},   {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSIGNMENT}, {3, ast::NodeType::VARIABLE_DEFINITION},
              {3, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"string s = \"Hello World!\""};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle type declarations") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::TYPE_DECLARATION},
              {3, ast::NodeType::TYPE_MEMBER},
        };
        std::vector<std::string> program = {"type MyType {", "int t", "}"};
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
