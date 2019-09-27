#include "ParserTest.h"

#include <catch2/catch.hpp>

TEST_CASE("Parser can handle 'fun helloWorld ( ) { }  '") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STMTS},
          {2, GrammarSymbol::STMT},
          {3, GrammarSymbol::FUNCTION},
          {4, GrammarSymbol::FUN},
          {4, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::LEFT_PARAN},
          {4, GrammarSymbol::FUNCTION_HEADER},
          {5, GrammarSymbol::RIGHT_PARAN},
          {5, GrammarSymbol::FUNCTION_RETURN},
          {6, GrammarSymbol::LEFT_CURLY_BRACE},
          {6, GrammarSymbol::FUNCTION_BODY},
          {7, GrammarSymbol::RIGHT_CURLY_BRACE},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"fun helloWorld ( ) { }  "};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 'fun hello ( int i ) float { }  '") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},       {1, GrammarSymbol::STMTS},
          {2, GrammarSymbol::STMT},          {3, GrammarSymbol::FUNCTION},
          {4, GrammarSymbol::FUN},           {4, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::LEFT_PARAN},    {4, GrammarSymbol::FUNCTION_HEADER},
          {5, GrammarSymbol::FUNCTION_ARGS}, {6, GrammarSymbol::VARIABLE_DEFINITION},
          {7, GrammarSymbol::DATA_TYPE},     {7, GrammarSymbol::VARIABLE_NAME},
          {5, GrammarSymbol::RIGHT_PARAN},   {5, GrammarSymbol::FUNCTION_RETURN},
          {6, GrammarSymbol::DATA_TYPE},     {6, GrammarSymbol::LEFT_CURLY_BRACE},
          {6, GrammarSymbol::FUNCTION_BODY}, {7, GrammarSymbol::RIGHT_CURLY_BRACE},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"fun hello ( int i ) float { }  "};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 'fun hello ( int i , bool b ) float { }  '") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0,  GrammarSymbol::PROGRAM},
          {1,  GrammarSymbol::STMTS},
          {2,  GrammarSymbol::STMT},
          {3,  GrammarSymbol::FUNCTION},
          {4,  GrammarSymbol::FUN},
          {4,  GrammarSymbol::VARIABLE_NAME},
          {4,  GrammarSymbol::LEFT_PARAN},
          {4,  GrammarSymbol::FUNCTION_HEADER},
          {5,  GrammarSymbol::FUNCTION_ARGS},
          {6,  GrammarSymbol::FUNCTION_ARGS},
          {7,  GrammarSymbol::VARIABLE_DEFINITION},
          {8,  GrammarSymbol::DATA_TYPE},
          {8,  GrammarSymbol::VARIABLE_NAME},
          {6,  GrammarSymbol::COMMA},
          {6,  GrammarSymbol::VARIABLE_DEFINITION},
          {7,  GrammarSymbol::DATA_TYPE},
          {7,  GrammarSymbol::VARIABLE_NAME},
          {5,  GrammarSymbol::RIGHT_PARAN},
          {5,  GrammarSymbol::FUNCTION_RETURN},
          {6,  GrammarSymbol::DATA_TYPE},
          {6,  GrammarSymbol::LEFT_CURLY_BRACE},
          {6,  GrammarSymbol::FUNCTION_BODY},
          {7,  GrammarSymbol::RIGHT_CURLY_BRACE},
          {1,  GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"fun hello ( int i , bool b ) float { }  "};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 'extern fun printf ( ) ;  '") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STMTS},
          {2, GrammarSymbol::STMT},
          {3, GrammarSymbol::EXTERNAL_FUNCTION},
          {4, GrammarSymbol::EXTERN},
          {4, GrammarSymbol::FUN},
          {4, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::LEFT_PARAN},
          {4, GrammarSymbol::EXTERNAL_FUNCTION_ARGS},
          {5, GrammarSymbol::EXTERNAL_FUNCTION_RETURN},
          {6, GrammarSymbol::RIGHT_PARAN},
          {3, GrammarSymbol::SEMICOLON},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"extern fun printf ( ) ;  "};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 'extern fun calc ( ) int ;  '") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STMTS},
          {2, GrammarSymbol::STMT},
          {3, GrammarSymbol::EXTERNAL_FUNCTION},
          {4, GrammarSymbol::EXTERN},
          {4, GrammarSymbol::FUN},
          {4, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::LEFT_PARAN},
          {4, GrammarSymbol::EXTERNAL_FUNCTION_ARGS},
          {5, GrammarSymbol::EXTERNAL_FUNCTION_RETURN},
          {6, GrammarSymbol::RIGHT_PARAN},
          {6, GrammarSymbol::DATA_TYPE},
          {3, GrammarSymbol::SEMICOLON},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"extern fun calc ( ) int ;  "};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 'extern fun calc ( float i ) int ;  '") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STMTS},
          {2, GrammarSymbol::STMT},
          {3, GrammarSymbol::EXTERNAL_FUNCTION},
          {4, GrammarSymbol::EXTERN},
          {4, GrammarSymbol::FUN},
          {4, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::LEFT_PARAN},
          {4, GrammarSymbol::EXTERNAL_FUNCTION_ARGS},
          {5, GrammarSymbol::FUNCTION_ARGS},
          {6, GrammarSymbol::VARIABLE_DEFINITION},
          {7, GrammarSymbol::DATA_TYPE},
          {7, GrammarSymbol::VARIABLE_NAME},
          {5, GrammarSymbol::EXTERNAL_FUNCTION_RETURN},
          {6, GrammarSymbol::RIGHT_PARAN},
          {6, GrammarSymbol::DATA_TYPE},
          {3, GrammarSymbol::SEMICOLON},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"extern fun calc ( float i ) int ;  "};
    assertProgramCreatesParseTree(program, parseTree);
}
