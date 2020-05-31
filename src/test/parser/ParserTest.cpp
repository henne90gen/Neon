#include <catch2/catch.hpp>

#include "ParserTest.h"

ParseTreeNode *createParseTree(const std::vector<std::pair<int, GrammarSymbol>> &tree, int &index,
                               int indentation = 0) {
    auto firstLine = tree[index];
    auto node = new ParseTreeNode();
    node->symbol = firstLine.second;
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

void assertParserAccepts(const std::vector<std::string> &program) {
    CodeProvider *cp = new StringCodeProvider(program, true);
    auto tokens = std::vector<Token>();
    Lexer lexer(cp, false);
    Parser parser(lexer, tokens, false);

    ParseTreeNode *actual = parser.createParseTree();
    REQUIRE(actual != nullptr);
}

TEST_CASE("Parser Basics") {
    SECTION("can handle two lines") {
        std::vector<std::string> program = {"int a = 1 + 1", "int b = a + 2"};
        assertParserAccepts(program);
    }

    SECTION("can handle return statement with function call") {
        std::vector<std::string> program = {"return hello()"};
        assertParserAccepts(program);
    }

    SECTION("can handle return statements with function calls and variable arguments") {
        std::vector<std::string> program = {"return hello(world - 1)"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'if true { } else { }'") {
        std::vector<std::string> program = {"if true { } else { }"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'if true { }'") {
        std::vector<std::string> program = {"if true { }"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'for int i = 0 ; i < 10 ; i = i + 1 { }'") {
        std::vector<std::string> program = {"for int i = 0; i< 10; i = i + 1 { }"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'int[5] a \n a[0] = 5 \n a[1] = a[0]'") {
        std::vector<std::string> program = {"int[5] a", "a[0] = 5", "a[1] = a[0]"};
        assertParserAccepts(program);
    }

    SECTION("can handle 'import \"test\"") {
        std::vector<std::string> program = {"import \"test\""};
        assertParserAccepts(program);
    }

    SECTION("can handle 'string s = \"Hello World!\"'") {
        std::vector<std::string> program = {"string s = \"Hello World!\""};
        assertParserAccepts(program);
    }

    SECTION("can handle complex types") {
        std::vector<std::string> program = {"type MyType {",       "int t",   "}",          "fun main() int {",
                                            "MyType t = MyType()", "t.i = 5", "return t.i", "}"};
        assertParserAccepts(program);
    }
}
