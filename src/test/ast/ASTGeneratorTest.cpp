#include "ASTTestHelper.h"

#include <catch2/catch.hpp>

TEST_CASE("ASTGenerator can handle arithmetic operations") {
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
    std::vector<std::string> program = {"int a = 1 + 1 ;", "int a = 1 - 1 ;", "int a = 1 / 1 ;", "int a = 1 * 1 ;"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("ASTGenerator can handle 'fun hello ( ) { }'") {
    std::vector<AstNodeSpec> spec = {
          {0, ast::NodeType::SEQUENCE},
          {1, ast::NodeType::STATEMENT},
          {2, ast::NodeType::FUNCTION},
          {3, ast::NodeType::SEQUENCE},
    };
    std::vector<std::string> program = {"fun hello ( ) { }"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("Function call") {
    std::vector<AstNodeSpec> spec = {
          {0, ast::NodeType::SEQUENCE},
          {1, ast::NodeType::STATEMENT},
          {2, ast::NodeType::CALL},
    };
    std::vector<std::string> program = {"hello ( ) ;"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("Function call with argument") {
    std::vector<AstNodeSpec> spec = {
          {0, ast::NodeType::SEQUENCE},
          {1, ast::NodeType::STATEMENT},
          {2, ast::NodeType::CALL},
          {3, ast::NodeType::LITERAL},
    };
    std::vector<std::string> program = {"hello ( 1 ) ;"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("External function") {
    std::vector<AstNodeSpec> spec = {
          {0, ast::NodeType::SEQUENCE},
          {1, ast::NodeType::STATEMENT},
          {2, ast::NodeType::FUNCTION},
    };
    std::vector<std::string> program = {"extern fun hello();"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("External function with return type") {
    std::vector<AstNodeSpec> spec = {
          {0, ast::NodeType::SEQUENCE},
          {1, ast::NodeType::STATEMENT},
          {2, ast::NodeType::FUNCTION},
    };
    std::vector<std::string> program = {"extern fun hello() int;"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("External function with argument") {
    std::vector<AstNodeSpec> spec = {
          {0, ast::NodeType::SEQUENCE},
          {1, ast::NodeType::STATEMENT},
          {2, ast::NodeType::FUNCTION},
          {3, ast::NodeType::VARIABLE_DEFINITION},
    };
    std::vector<std::string> program = {"extern fun hello(int i);"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("ASTGenerator can handle 'if ( true ) { }'") {
    std::vector<AstNodeSpec> spec = {
          {0, ast::NodeType::SEQUENCE},
          {1, ast::NodeType::STATEMENT},
          {2, ast::NodeType::IF_STATEMENT},
          {3, ast::NodeType::LITERAL},
    };
    std::vector<std::string> program = {"if ( true ) { }"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("ASTGenerator can handle 'if ( true ) { } else { }'") {
    std::vector<AstNodeSpec> spec = {
          {0, ast::NodeType::SEQUENCE},
          {1, ast::NodeType::STATEMENT},
          {2, ast::NodeType::IF_STATEMENT},
          {3, ast::NodeType::LITERAL},
    };
    std::vector<std::string> program = {"if ( true ) { } else { }"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("ASTGenerator can handle relational operations") {
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
    std::vector<std::string> program = {"bool a = 1 == 0 ;", "bool b = 1 <= 0 ;", "bool c = 1 < 0 ;",
                                        "bool d = 1 >= 0 ;", "bool e = 1 > 0 ;",  "bool f = 1 != 0 ;"};
    assertProgramCreatesAst(program, spec);
}