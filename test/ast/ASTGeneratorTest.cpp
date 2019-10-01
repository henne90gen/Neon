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
