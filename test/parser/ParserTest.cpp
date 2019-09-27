#include "ParserTest.h"

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

TEST_CASE("Parser can handle two lines") {
    std::vector<std::pair<int, GrammarSymbol>> parseTree = {
          {0, GrammarSymbol::PROGRAM},
          {1, GrammarSymbol::STMTS},
          {2, GrammarSymbol::STMTS},
          {3, GrammarSymbol::STMT},
          {4, GrammarSymbol::ASSIGNMENT},
          {5, GrammarSymbol::VARIABLE_DEFINITION},
          {6, GrammarSymbol::DATA_TYPE},
          {6, GrammarSymbol::VARIABLE_NAME},
          {5, GrammarSymbol::SINGLE_EQUALS},
          {5, GrammarSymbol::EXPR},
          {6, GrammarSymbol::DISJUNCTION},
          {7, GrammarSymbol::CONJUNCTION},
          {8, GrammarSymbol::NEGATION},
          {9, GrammarSymbol::RELATION},
          {10, GrammarSymbol::SUM},
          {11, GrammarSymbol::SUM},
          {12, GrammarSymbol::TERM},
          {13, GrammarSymbol::FACTOR},
          {14, GrammarSymbol::INTEGER},
          {11, GrammarSymbol::PLUS},
          {11, GrammarSymbol::TERM},
          {12, GrammarSymbol::FACTOR},
          {13, GrammarSymbol::INTEGER},
          {4, GrammarSymbol::SEMICOLON},
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
          {13, GrammarSymbol::VARIABLE_NAME},
          {10, GrammarSymbol::PLUS},
          {10, GrammarSymbol::TERM},
          {11, GrammarSymbol::FACTOR},
          {12, GrammarSymbol::INTEGER},
          {3, GrammarSymbol::SEMICOLON},
          {1, GrammarSymbol::ENDOFFILE},
    };
    std::vector<std::string> program = {"int a = 1 + 1 ; int b = a + 2 ;  "};
    assertProgramCreatesParseTree(program, parseTree);
}
