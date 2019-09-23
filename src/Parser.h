#pragma once

#include <utility>

#include "Grammar.h"
#include "Lexer.h"

class ParseTreeNode {
  public:
    explicit ParseTreeNode(GrammarSymbol symbol) : symbol(symbol) {}

    ParseTreeNode(GrammarSymbol symbol, Token token) : symbol(symbol), token(std::move(token)) {}

    GrammarSymbol symbol;
    Token token;
    std::vector<ParseTreeNode *> children = {};
    std::vector<Token> program = {};
};

class Parser {
  public:
    explicit Parser(Lexer lexer, bool verbose = false) : lexer(std::move(lexer)), verbose(verbose) {}

    ParseTreeNode *createParseTree();

  private:
    Lexer lexer;
    bool verbose;
    std::vector<Token> program = {};

    void executeShift(Token &token, std::vector<int> &states, StateTransition &action,
                      std::vector<ParseTreeNode *> &nodes);

    void executeGoto(std::vector<int> &states, StateTransition &action);

    void executeReduce(std::vector<int> &states, StateTransition &action, std::vector<ParseTreeNode *> &nodes);

    std::optional<StateTransition> getNextAction(int rowIndex, int columnIndex);
};

void printParseTree(ParseTreeNode *node, int indentation = 0);

void printParseTreeTestCase(ParseTreeNode *node, int indentation = 0);
