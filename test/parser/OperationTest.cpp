#include "ParserTest.h"

#include <catch2/catch.hpp>

TEST_CASE("Parser can handle 'int i = 1 + 1.5 ;  '") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STMTS},
          {2, GrammarSymbol::STMT},
          {3, GrammarSymbol::ASSIGNMENT},
          {4, GrammarSymbol::VARIABLE_DEFINITION},
          {5, GrammarSymbol::DATA_TYPE},
          {5, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::SINGLE_EQUALS},
          {4, GrammarSymbol::EXPR},
          {5, GrammarSymbol::DISJUNCTION},
          {6, GrammarSymbol::CONJUNCTION},
          {7, GrammarSymbol::NEGATION},
          {8, GrammarSymbol::RELATION},
          {9, GrammarSymbol::SUM},
          {10, GrammarSymbol::SUM},
          {11, GrammarSymbol::TERM},
          {12, GrammarSymbol::FACTOR},
          {13, GrammarSymbol::INTEGER},
          {10, GrammarSymbol::PLUS},
          {10, GrammarSymbol::TERM},
          {11, GrammarSymbol::FACTOR},
          {12, GrammarSymbol::FLOAT},
          {3, GrammarSymbol::SEMICOLON},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"int i = 1 + 1.5 ;  "};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 'int i = 4 - 5 ;  '") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STMTS},
          {2, GrammarSymbol::STMT},
          {3, GrammarSymbol::ASSIGNMENT},
          {4, GrammarSymbol::VARIABLE_DEFINITION},
          {5, GrammarSymbol::DATA_TYPE},
          {5, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::SINGLE_EQUALS},
          {4, GrammarSymbol::EXPR},
          {5, GrammarSymbol::DISJUNCTION},
          {6, GrammarSymbol::CONJUNCTION},
          {7, GrammarSymbol::NEGATION},
          {8, GrammarSymbol::RELATION},
          {9, GrammarSymbol::SUM},
          {10, GrammarSymbol::SUM},
          {11, GrammarSymbol::TERM},
          {12, GrammarSymbol::FACTOR},
          {13, GrammarSymbol::INTEGER},
          {10, GrammarSymbol::MINUS},
          {10, GrammarSymbol::TERM},
          {11, GrammarSymbol::FACTOR},
          {12, GrammarSymbol::INTEGER},
          {3, GrammarSymbol::SEMICOLON},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"int i = 4 - 5 ;  "};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 'bool b = not true ;  '") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STMTS},
          {2, GrammarSymbol::STMT},
          {3, GrammarSymbol::ASSIGNMENT},
          {4, GrammarSymbol::VARIABLE_DEFINITION},
          {5, GrammarSymbol::DATA_TYPE},
          {5, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::SINGLE_EQUALS},
          {4, GrammarSymbol::EXPR},
          {5, GrammarSymbol::DISJUNCTION},
          {6, GrammarSymbol::CONJUNCTION},
          {7, GrammarSymbol::NEGATION},
          {8, GrammarSymbol::NOT},
          {8, GrammarSymbol::RELATION},
          {9, GrammarSymbol::SUM},
          {10, GrammarSymbol::TERM},
          {11, GrammarSymbol::FACTOR},
          {12, GrammarSymbol::TRUE},
          {3, GrammarSymbol::SEMICOLON},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"bool b = not true ;  "};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 'bool b = 7 < 8 ;  '") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STMTS},
          {2, GrammarSymbol::STMT},
          {3, GrammarSymbol::ASSIGNMENT},
          {4, GrammarSymbol::VARIABLE_DEFINITION},
          {5, GrammarSymbol::DATA_TYPE},
          {5, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::SINGLE_EQUALS},
          {4, GrammarSymbol::EXPR},
          {5, GrammarSymbol::DISJUNCTION},
          {6, GrammarSymbol::CONJUNCTION},
          {7, GrammarSymbol::NEGATION},
          {8, GrammarSymbol::RELATION},
          {9, GrammarSymbol::SUM},
          {10, GrammarSymbol::TERM},
          {11, GrammarSymbol::FACTOR},
          {12, GrammarSymbol::INTEGER},
          {9, GrammarSymbol::LESS_THAN},
          {9, GrammarSymbol::SUM},
          {10, GrammarSymbol::TERM},
          {11, GrammarSymbol::FACTOR},
          {12, GrammarSymbol::INTEGER},
          {3, GrammarSymbol::SEMICOLON},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"bool b = 7 < 8 ;  "};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 'bool b = not ( 1 < 2 ) ;  '") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STMTS},
          {2, GrammarSymbol::STMT},
          {3, GrammarSymbol::ASSIGNMENT},
          {4, GrammarSymbol::VARIABLE_DEFINITION},
          {5, GrammarSymbol::DATA_TYPE},
          {5, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::SINGLE_EQUALS},
          {4, GrammarSymbol::EXPR},
          {5, GrammarSymbol::DISJUNCTION},
          {6, GrammarSymbol::CONJUNCTION},
          {7, GrammarSymbol::NEGATION},
          {8, GrammarSymbol::NOT},
          {8, GrammarSymbol::RELATION},
          {9, GrammarSymbol::SUM},
          {10, GrammarSymbol::TERM},
          {11, GrammarSymbol::FACTOR},
          {12, GrammarSymbol::LEFT_PARAN},
          {12, GrammarSymbol::EXPR},
          {13, GrammarSymbol::DISJUNCTION},
          {14, GrammarSymbol::CONJUNCTION},
          {15, GrammarSymbol::NEGATION},
          {16, GrammarSymbol::RELATION},
          {17, GrammarSymbol::SUM},
          {18, GrammarSymbol::TERM},
          {19, GrammarSymbol::FACTOR},
          {20, GrammarSymbol::INTEGER},
          {17, GrammarSymbol::LESS_THAN},
          {17, GrammarSymbol::SUM},
          {18, GrammarSymbol::TERM},
          {19, GrammarSymbol::FACTOR},
          {20, GrammarSymbol::INTEGER},
          {12, GrammarSymbol::RIGHT_PARAN},
          {3, GrammarSymbol::SEMICOLON},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"bool b = not ( 1 < 2 ) ;  "};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 'bool b = true and false ;  '") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STMTS},
          {2, GrammarSymbol::STMT},
          {3, GrammarSymbol::ASSIGNMENT},
          {4, GrammarSymbol::VARIABLE_DEFINITION},
          {5, GrammarSymbol::DATA_TYPE},
          {5, GrammarSymbol::VARIABLE_NAME},
          {4, GrammarSymbol::SINGLE_EQUALS},
          {4, GrammarSymbol::EXPR},
          {5, GrammarSymbol::DISJUNCTION},
          {6, GrammarSymbol::CONJUNCTION},
          {7, GrammarSymbol::CONJUNCTION},
          {8, GrammarSymbol::NEGATION},
          {9, GrammarSymbol::RELATION},
          {10, GrammarSymbol::SUM},
          {11, GrammarSymbol::TERM},
          {12, GrammarSymbol::FACTOR},
          {13, GrammarSymbol::TRUE},
          {7, GrammarSymbol::AND},
          {7, GrammarSymbol::NEGATION},
          {8, GrammarSymbol::RELATION},
          {9, GrammarSymbol::SUM},
          {10, GrammarSymbol::TERM},
          {11, GrammarSymbol::FACTOR},
          {12, GrammarSymbol::FALSE},
          {3, GrammarSymbol::SEMICOLON},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"bool b = true and false ;  "};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 'a = 5 ;'") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0,  GrammarSymbol::PROGRAM},
          {1,  GrammarSymbol::STMTS},
          {2,  GrammarSymbol::STMT},
          {3,  GrammarSymbol::ASSIGNMENT},
          {4,  GrammarSymbol::VARIABLE_NAME},
          {4,  GrammarSymbol::SINGLE_EQUALS},
          {4,  GrammarSymbol::EXPR},
          {5,  GrammarSymbol::DISJUNCTION},
          {6,  GrammarSymbol::CONJUNCTION},
          {7,  GrammarSymbol::NEGATION},
          {8,  GrammarSymbol::RELATION},
          {9,  GrammarSymbol::SUM},
          {10,  GrammarSymbol::TERM},
          {11,  GrammarSymbol::FACTOR},
          {12,  GrammarSymbol::INTEGER},
          {3,  GrammarSymbol::SEMICOLON},
          {1,  GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"a = 5 ;"};
    assertProgramCreatesParseTree(program, parseTree);
}
