#include "ASTTestHelper.h"

#include <catch2/catch.hpp>

TEST_CASE("ASTGenerator can handle 'fun hello ( ) { }'") {
    std::vector<AstNodeSpec> spec = {
          {0, AstNode::SEQUENCE},
          {1, AstNode::STATEMENT},
          {2, AstNode::FUNCTION},
          {3, AstNode::SEQUENCE},
    };
    std::vector<std::string> program = {"fun hello ( ) { }"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("ASTGenerator can handle 'int a = 1 + 1 ;'") {
    std::vector<AstNodeSpec> spec = {
          {0, AstNode::SEQUENCE},         {1, AstNode::STATEMENT},
          {2, AstNode::ASSIGNMENT},       {3, AstNode::VARIABLE_DEFINITION},
          {3, AstNode::BINARY_OPERATION}, {4, AstNode::LITERAL},
          {4, AstNode::LITERAL},
    };
    std::vector<std::string> program = {"int a = 1 + 1 ;"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("ASTGenerator can handle 'int a = 1 + 1 ; int b = a + 2 ;'") {
    std::vector<AstNodeSpec> spec = {
          {0, AstNode::SEQUENCE},         {1, AstNode::STATEMENT},
          {2, AstNode::ASSIGNMENT},       {3, AstNode::VARIABLE_DEFINITION},
          {3, AstNode::BINARY_OPERATION}, {4, AstNode::LITERAL},
          {4, AstNode::LITERAL},          {1, AstNode::STATEMENT},
          {2, AstNode::ASSIGNMENT},       {3, AstNode::VARIABLE_DEFINITION},
          {3, AstNode::BINARY_OPERATION}, {4, AstNode::VARIABLE},
          {4, AstNode::LITERAL},
    };
    std::vector<std::string> program = {"int a = 1 + 1 ; int b = a + 2 ;"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("Function call") {
    std::vector<AstNodeSpec> spec = {
          {0, AstNode::SEQUENCE},
          {1, AstNode::STATEMENT},
          {2, AstNode::CALL},
    };
    std::vector<std::string> program = {"hello ( ) ;"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("Function call with argument") {
    std::vector<AstNodeSpec> spec = {
          {0, AstNode::SEQUENCE},
          {1, AstNode::STATEMENT},
          {2, AstNode::CALL},
          {3, AstNode::LITERAL},
    };
    std::vector<std::string> program = {"hello ( 1 ) ;"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("External function") {
    std::vector<AstNodeSpec> spec = {
          {0, AstNode::SEQUENCE},
          {1, AstNode::STATEMENT},
          {2, AstNode::FUNCTION},
    };
    std::vector<std::string> program = {"extern fun hello();"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("External function with return type") {
    std::vector<AstNodeSpec> spec = {
          {0, AstNode::SEQUENCE},
          {1, AstNode::STATEMENT},
          {2, AstNode::FUNCTION},
    };
    std::vector<std::string> program = {"extern fun hello() int;"};
    assertProgramCreatesAst(program, spec);
}

TEST_CASE("External function with argument") {
    std::vector<AstNodeSpec> spec = {
          {0, AstNode::SEQUENCE},
          {1, AstNode::STATEMENT},
          {2, AstNode::FUNCTION},
          {3, AstNode::VARIABLE_DEFINITION},
    };
    std::vector<std::string> program = {"extern fun hello(int i);"};
    assertProgramCreatesAst(program, spec);
}
