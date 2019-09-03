#include "Parser.h"

#include <catch2/catch.hpp>

ParseTreeNode *createParseTree(const std::vector<std::pair<int, GrammarSymbol>> &tree, int &index,
                               int indentation = 0) {
    auto firstLine = tree[index];
    auto node = new ParseTreeNode(firstLine.second);
    while (index + 1 < tree.size() && tree[index + 1].first > indentation) {
        index++;
        auto child = createParseTree(tree, index, indentation + 1);
        node->children.push_back(child);
    }
    return node;
}

void assertParseTreesAreEqual(ParseTreeNode *node1, ParseTreeNode *node2) {
    INFO("Node " + to_string(node1->symbol) + " | Node " + to_string(node2->symbol))
    REQUIRE(node1->symbol == node2->symbol);
    REQUIRE(node1->children.size() == node2->children.size());
    for (unsigned long i = 0; i < node1->children.size(); i++) {
        assertParseTreesAreEqual(node1->children[i], node2->children[i]);
    }
}

void assertProgramCreatesParseTree(const std::vector<std::string> &program,
                                   const std::vector<std::pair<int, GrammarSymbol>> &tree) {
    int index = 0;
    ParseTreeNode *expected = createParseTree(tree, index);
    CodeProvider *cp = new StringCodeProvider(program);
    Lexer lexer(cp, false);
    Parser parser(lexer, false);

    ParseTreeNode *actual = parser.createParseTree();
    assertParseTreesAreEqual(expected, actual);
}

TEST_CASE("Parser can handle '1 + 1.5;'") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},     {1, GrammarSymbol::STMTS},     {2, GrammarSymbol::STMTS},
          {3, GrammarSymbol::STMT},        {4, GrammarSymbol::EXPR},      {5, GrammarSymbol::DISJUNCTION},
          {6, GrammarSymbol::CONJUNCTION}, {7, GrammarSymbol::NEGATION},  {8, GrammarSymbol::RELATION},
          {9, GrammarSymbol::SUM},         {10, GrammarSymbol::SUM},      {11, GrammarSymbol::TERM},
          {12, GrammarSymbol::FACTOR},     {13, GrammarSymbol::INTEGER},  {10, GrammarSymbol::PLUS},
          {10, GrammarSymbol::TERM},       {11, GrammarSymbol::FACTOR},   {12, GrammarSymbol::FLOAT},
          {2, GrammarSymbol::SEMICOLON},   {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"1 + 1.5;"};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 4 - 5;") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},     {1, GrammarSymbol::STMTS},     {2, GrammarSymbol::STMTS},
          {3, GrammarSymbol::STMT},        {4, GrammarSymbol::EXPR},      {5, GrammarSymbol::DISJUNCTION},
          {6, GrammarSymbol::CONJUNCTION}, {7, GrammarSymbol::NEGATION},  {8, GrammarSymbol::RELATION},
          {9, GrammarSymbol::SUM},         {10, GrammarSymbol::SUM},      {11, GrammarSymbol::TERM},
          {12, GrammarSymbol::FACTOR},     {13, GrammarSymbol::INTEGER},  {10, GrammarSymbol::MINUS},
          {10, GrammarSymbol::TERM},       {11, GrammarSymbol::FACTOR},   {12, GrammarSymbol::INTEGER},
          {2, GrammarSymbol::SEMICOLON},   {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"4 - 5;"};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 4-5;") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},     {1, GrammarSymbol::STMTS},     {2, GrammarSymbol::STMTS},
          {3, GrammarSymbol::STMT},        {4, GrammarSymbol::EXPR},      {5, GrammarSymbol::DISJUNCTION},
          {6, GrammarSymbol::CONJUNCTION}, {7, GrammarSymbol::NEGATION},  {8, GrammarSymbol::RELATION},
          {9, GrammarSymbol::SUM},         {10, GrammarSymbol::SUM},      {11, GrammarSymbol::TERM},
          {12, GrammarSymbol::FACTOR},     {13, GrammarSymbol::INTEGER},  {10, GrammarSymbol::MINUS},
          {10, GrammarSymbol::TERM},       {11, GrammarSymbol::FACTOR},   {12, GrammarSymbol::INTEGER},
          {2, GrammarSymbol::SEMICOLON},   {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"4-5;"};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle 'not true;'") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},     {1, GrammarSymbol::STMTS},    {2, GrammarSymbol::STMTS},
          {3, GrammarSymbol::STMT},        {4, GrammarSymbol::EXPR},     {5, GrammarSymbol::DISJUNCTION},
          {6, GrammarSymbol::CONJUNCTION}, {7, GrammarSymbol::NEGATION}, {8, GrammarSymbol::NOT},
          {8, GrammarSymbol::RELATION},    {9, GrammarSymbol::SUM},      {10, GrammarSymbol::TERM},
          {11, GrammarSymbol::FACTOR},     {12, GrammarSymbol::TRUE},    {2, GrammarSymbol::SEMICOLON},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"not true;"};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle '7 < 8;") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},     {1, GrammarSymbol::STMTS},     {2, GrammarSymbol::STMTS},
          {3, GrammarSymbol::STMT},        {4, GrammarSymbol::EXPR},      {5, GrammarSymbol::DISJUNCTION},
          {6, GrammarSymbol::CONJUNCTION}, {7, GrammarSymbol::NEGATION},  {8, GrammarSymbol::RELATION},
          {9, GrammarSymbol::SUM},         {10, GrammarSymbol::TERM},     {11, GrammarSymbol::FACTOR},
          {12, GrammarSymbol::INTEGER},    {9, GrammarSymbol::LESS_THAN}, {9, GrammarSymbol::SUM},
          {10, GrammarSymbol::TERM},       {11, GrammarSymbol::FACTOR},   {12, GrammarSymbol::INTEGER},
          {2, GrammarSymbol::SEMICOLON},   {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"7 < 8;"};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle two line program") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},     {1, GrammarSymbol::STMTS},       {2, GrammarSymbol::STMTS},
          {3, GrammarSymbol::STMT},        {4, GrammarSymbol::EXPR},        {5, GrammarSymbol::DISJUNCTION},
          {6, GrammarSymbol::CONJUNCTION}, {7, GrammarSymbol::NEGATION},    {8, GrammarSymbol::RELATION},
          {9, GrammarSymbol::SUM},         {10, GrammarSymbol::SUM},        {11, GrammarSymbol::TERM},
          {12, GrammarSymbol::FACTOR},     {13, GrammarSymbol::INTEGER},    {10, GrammarSymbol::PLUS},
          {10, GrammarSymbol::TERM},       {11, GrammarSymbol::FACTOR},     {12, GrammarSymbol::INTEGER},
          {2, GrammarSymbol::SEMICOLON},   {2, GrammarSymbol::STMT},        {3, GrammarSymbol::EXPR},
          {4, GrammarSymbol::DISJUNCTION}, {5, GrammarSymbol::CONJUNCTION}, {6, GrammarSymbol::NEGATION},
          {7, GrammarSymbol::RELATION},    {8, GrammarSymbol::SUM},         {9, GrammarSymbol::SUM},
          {10, GrammarSymbol::TERM},       {11, GrammarSymbol::FACTOR},     {12, GrammarSymbol::INTEGER},
          {9, GrammarSymbol::PLUS},        {9, GrammarSymbol::TERM},        {10, GrammarSymbol::FACTOR},
          {11, GrammarSymbol::INTEGER},    {2, GrammarSymbol::SEMICOLON},   {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"1 + 1;", "1 + 2;"};
    assertProgramCreatesParseTree(program, parseTree);
}

TEST_CASE("Parser can handle single statement without semicolon") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},  {1, GrammarSymbol::STMTS},       {2, GrammarSymbol::STMT},
          {3, GrammarSymbol::EXPR},     {4, GrammarSymbol::DISJUNCTION}, {5, GrammarSymbol::CONJUNCTION},
          {6, GrammarSymbol::NEGATION}, {7, GrammarSymbol::RELATION},    {8, GrammarSymbol::SUM},
          {9, GrammarSymbol::SUM},      {10, GrammarSymbol::TERM},       {11, GrammarSymbol::FACTOR},
          {12, GrammarSymbol::INTEGER}, {9, GrammarSymbol::PLUS},        {9, GrammarSymbol::TERM},
          {10, GrammarSymbol::FACTOR},  {11, GrammarSymbol::INTEGER},    {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"1 + 1"};
    assertProgramCreatesParseTree(program, parseTree);
}
