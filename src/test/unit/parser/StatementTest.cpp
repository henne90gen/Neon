#include <catch2/catch.hpp>

#include "ParserTestHelper.h"

TEST_CASE("Parser Statements") {
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

    SECTION("can handle 'if true { int i = 0 }'") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},     {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::IF_STATEMENT}, {3, ast::NodeType::LITERAL},
              {3, ast::NodeType::SEQUENCE},     {4, ast::NodeType::STATEMENT},
              {5, ast::NodeType::ASSIGNMENT},   {6, ast::NodeType::VARIABLE_DEFINITION},
              {6, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"if true {", "int i = 0", "}"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle 'if num == 1 { }'") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},         {1, ast::NodeType::STATEMENT}, {2, ast::NodeType::IF_STATEMENT},
              {3, ast::NodeType::BINARY_OPERATION}, {4, ast::NodeType::VARIABLE},  {4, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"if num == 1 {", "}"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle 'if true { int i = 0 } else { int j = 1 }'") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::IF_STATEMENT},
              {3, ast::NodeType::LITERAL},
              {3, ast::NodeType::SEQUENCE},
              {4, ast::NodeType::STATEMENT},
              {5, ast::NodeType::ASSIGNMENT},
              {6, ast::NodeType::VARIABLE_DEFINITION},
              {6, ast::NodeType::LITERAL},
              {3, ast::NodeType::SEQUENCE},
              {4, ast::NodeType::STATEMENT},
              {5, ast::NodeType::ASSIGNMENT},
              {6, ast::NodeType::VARIABLE_DEFINITION},
              {6, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"if true {", "int i = 0", "} else {", "int j = 1", "}"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle 'for int i = 0; i < 10; i = i + 1 { }'") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},      {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FOR_STATEMENT}, {3, ast::NodeType::STATEMENT},
              {4, ast::NodeType::ASSIGNMENT},    {5, ast::NodeType::VARIABLE_DEFINITION},
              {5, ast::NodeType::LITERAL},       {3, ast::NodeType::BINARY_OPERATION},
              {4, ast::NodeType::VARIABLE},      {4, ast::NodeType::LITERAL},
              {3, ast::NodeType::STATEMENT},     {4, ast::NodeType::ASSIGNMENT},
              {5, ast::NodeType::VARIABLE},      {5, ast::NodeType::BINARY_OPERATION},
              {6, ast::NodeType::VARIABLE},      {6, ast::NodeType::LITERAL},
              {3, ast::NodeType::SEQUENCE},
        };
        std::vector<std::string> program = {"for int i = 0; i < 10; i = i + 1 { }"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle 'for int i = 0; i < 10; i = i + 1 { int a = 0 }'") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},      {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::FOR_STATEMENT}, {3, ast::NodeType::STATEMENT},
              {4, ast::NodeType::ASSIGNMENT},    {5, ast::NodeType::VARIABLE_DEFINITION},
              {5, ast::NodeType::LITERAL},       {3, ast::NodeType::BINARY_OPERATION},
              {4, ast::NodeType::VARIABLE},      {4, ast::NodeType::LITERAL},
              {3, ast::NodeType::STATEMENT},     {4, ast::NodeType::ASSIGNMENT},
              {5, ast::NodeType::VARIABLE},      {5, ast::NodeType::BINARY_OPERATION},
              {6, ast::NodeType::VARIABLE},      {6, ast::NodeType::LITERAL},
              {3, ast::NodeType::SEQUENCE},      {4, ast::NodeType::STATEMENT},
              {5, ast::NodeType::ASSIGNMENT},    {6, ast::NodeType::VARIABLE_DEFINITION},
              {6, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"for int i = 0; i < 10; i = i + 1 {", "int a = 0", "}"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle assert statement") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::ASSERT},
              {3, ast::NodeType::LITERAL},
        };
        std::vector<std::string> program = {"assert true"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle comment") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::COMMENT},
        };
        std::vector<std::string> program = {"# this is a comment"};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }

    SECTION("can handle import statement") {
        std::vector<AstNodeSpec> spec = {
              {0, ast::NodeType::SEQUENCE},
              {1, ast::NodeType::STATEMENT},
              {2, ast::NodeType::IMPORT},
        };
        std::vector<std::string> program = {"import \"examples/functions.ne\""};
        assertProgramCreatesAstWithSimpleParser(program, spec);
    }
}