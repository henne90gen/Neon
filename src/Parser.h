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
};

class Parser {
  public:
    explicit Parser(Lexer &lexer, Program &program, bool verbose = false) : lexer(lexer), program(program), verbose(verbose) {}

    ParseTreeNode *createParseTree();

  private:
    Lexer &lexer;
    Program &program;
    bool verbose;

    void executeShift(Token &token, std::vector<int> &states, StateTransition &action,
                      std::vector<ParseTreeNode *> &nodes);

    void executeGoto(std::vector<int> &states, StateTransition &action);

    void executeReduce(std::vector<int> &states, StateTransition &action, std::vector<ParseTreeNode *> &nodes);

    std::optional<StateTransition> getNextAction(int rowIndex, int columnIndex);
};

void printParseTree(ParseTreeNode *node, int indentation = 0);

void printParseTreeTestCase(const ParseTreeNode *node, const Program &program, int indentation = 0);
