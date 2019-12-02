#include "ParserTest.h"

#include <catch2/catch.hpp>

TEST_CASE("Function definition") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},         {1, GrammarSymbol::STATEMENTS},
          {2, GrammarSymbol::STATEMENTS},      {3, GrammarSymbol::STATEMENT},
          {4, GrammarSymbol::FUNCTION},        {5, GrammarSymbol::FUN},
          {5, GrammarSymbol::VARIABLE_NAME},   {5, GrammarSymbol::LEFT_PARAN},
          {5, GrammarSymbol::FUNCTION_HEADER}, {6, GrammarSymbol::RIGHT_PARAN},
          {6, GrammarSymbol::FUNCTION_RETURN}, {7, GrammarSymbol::LEFT_CURLY_BRACE},
          {7, GrammarSymbol::FUNCTION_BODY},   {8, GrammarSymbol::RIGHT_CURLY_BRACE},
          {2, GrammarSymbol::STATEMENT},       {3, GrammarSymbol::NEW_LINE},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"fun helloWorld() { }"};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Function definition with argument and return type") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},         {1, GrammarSymbol::STATEMENTS},
          {2, GrammarSymbol::STATEMENTS},      {3, GrammarSymbol::STATEMENT},
          {4, GrammarSymbol::FUNCTION},        {5, GrammarSymbol::FUN},
          {5, GrammarSymbol::VARIABLE_NAME},   {5, GrammarSymbol::LEFT_PARAN},
          {5, GrammarSymbol::FUNCTION_HEADER}, {6, GrammarSymbol::FUNCTION_ARGS},
          {7, GrammarSymbol::DEFINITION},      {8, GrammarSymbol::VARIABLE_DEFINITION},
          {9, GrammarSymbol::DATA_TYPE},       {9, GrammarSymbol::VARIABLE_NAME},
          {6, GrammarSymbol::RIGHT_PARAN},     {6, GrammarSymbol::FUNCTION_RETURN},
          {7, GrammarSymbol::DATA_TYPE},       {7, GrammarSymbol::LEFT_CURLY_BRACE},
          {7, GrammarSymbol::FUNCTION_BODY},   {8, GrammarSymbol::RIGHT_CURLY_BRACE},
          {2, GrammarSymbol::STATEMENT},       {3, GrammarSymbol::NEW_LINE},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"fun hello(int i) float { }"};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Function definition with multiple arguments and return type") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STATEMENTS},
          {2, GrammarSymbol::STATEMENTS},
          {3, GrammarSymbol::STATEMENT},
          {4, GrammarSymbol::FUNCTION},
          {5, GrammarSymbol::FUN},
          {5, GrammarSymbol::VARIABLE_NAME},
          {5, GrammarSymbol::LEFT_PARAN},
          {5, GrammarSymbol::FUNCTION_HEADER},
          {6, GrammarSymbol::FUNCTION_ARGS},
          {7, GrammarSymbol::FUNCTION_ARGS},
          {8, GrammarSymbol::DEFINITION},
          {9, GrammarSymbol::VARIABLE_DEFINITION},
          {10, GrammarSymbol::DATA_TYPE},
          {10, GrammarSymbol::VARIABLE_NAME},
          {7, GrammarSymbol::COMMA},
          {7, GrammarSymbol::DEFINITION},
          {8, GrammarSymbol::VARIABLE_DEFINITION},
          {9, GrammarSymbol::DATA_TYPE},
          {9, GrammarSymbol::VARIABLE_NAME},
          {6, GrammarSymbol::RIGHT_PARAN},
          {6, GrammarSymbol::FUNCTION_RETURN},
          {7, GrammarSymbol::DATA_TYPE},
          {7, GrammarSymbol::LEFT_CURLY_BRACE},
          {7, GrammarSymbol::FUNCTION_BODY},
          {8, GrammarSymbol::RIGHT_CURLY_BRACE},
          {2, GrammarSymbol::STATEMENT},
          {3, GrammarSymbol::NEW_LINE},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"fun hello(int i, bool b) float { }"};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("External function definition") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STATEMENTS},
          {2, GrammarSymbol::STATEMENT},
          {3, GrammarSymbol::EXTERNAL_FUNCTION},
          {4, GrammarSymbol::EXTERN},
          {4, GrammarSymbol::FUN},
          {4, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::LEFT_PARAN},
          {4, GrammarSymbol::EXTERNAL_FUNCTION_HEADER},
          {5, GrammarSymbol::EXTERNAL_FUNCTION_RETURN},
          {6, GrammarSymbol::RIGHT_PARAN},
          {3, GrammarSymbol::NEW_LINE},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"extern fun calc()"};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("External function definition with return type") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STATEMENTS},
          {2, GrammarSymbol::STATEMENT},
          {3, GrammarSymbol::EXTERNAL_FUNCTION},
          {4, GrammarSymbol::EXTERN},
          {4, GrammarSymbol::FUN},
          {4, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::LEFT_PARAN},
          {4, GrammarSymbol::EXTERNAL_FUNCTION_HEADER},
          {5, GrammarSymbol::EXTERNAL_FUNCTION_RETURN},
          {6, GrammarSymbol::RIGHT_PARAN},
          {6, GrammarSymbol::DATA_TYPE},
          {3, GrammarSymbol::NEW_LINE},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"extern fun calc() int"};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("External function definition with arguments and return type") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STATEMENTS},
          {2, GrammarSymbol::STATEMENT},
          {3, GrammarSymbol::EXTERNAL_FUNCTION},
          {4, GrammarSymbol::EXTERN},
          {4, GrammarSymbol::FUN},
          {4, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::LEFT_PARAN},
          {4, GrammarSymbol::EXTERNAL_FUNCTION_HEADER},
          {5, GrammarSymbol::FUNCTION_ARGS},
          {6, GrammarSymbol::DEFINITION},
          {7, GrammarSymbol::VARIABLE_DEFINITION},
          {8, GrammarSymbol::DATA_TYPE},
          {8, GrammarSymbol::VARIABLE_NAME},
          {5, GrammarSymbol::EXTERNAL_FUNCTION_RETURN},
          {6, GrammarSymbol::RIGHT_PARAN},
          {6, GrammarSymbol::DATA_TYPE},
          {3, GrammarSymbol::NEW_LINE},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"extern fun calc(int i) int"};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Function call without arguments") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},     {1, GrammarSymbol::STATEMENTS},    {2, GrammarSymbol::STATEMENT},
          {3, GrammarSymbol::CALL},        {4, GrammarSymbol::VARIABLE_NAME}, {4, GrammarSymbol::LEFT_PARAN},
          {4, GrammarSymbol::CALL_HEADER}, {5, GrammarSymbol::RIGHT_PARAN},   {3, GrammarSymbol::NEW_LINE},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"hello()"};
    assertProgramCreatesParseTree(program, parseTree);
}
