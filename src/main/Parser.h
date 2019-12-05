#pragma once

#include "Grammar.h"
#include "Lexer.h"
#include "ParseTreeNode.h"

class Parser {
  public:
    explicit Parser(Lexer &lexer, Module *program, bool verbose = false)
        : lexer(lexer), program(program), verbose(verbose) {}

    ParseTreeNode *createParseTree();

  private:
    Lexer &lexer;
    Module *program;
    bool verbose;

    void executeShift(Token &token, std::vector<int> &states, StateTransition &action,
                      std::vector<ParseTreeNode *> &nodes);

    static void executeGoto(std::vector<int> &states, StateTransition &action);

    void executeReduce(std::vector<int> &states, StateTransition &action, std::vector<ParseTreeNode *> &nodes);

    std::optional<StateTransition> getNextAction(int rowIndex, int columnIndex);
};

void printParseTree(ParseTreeNode *node, int indentation = 0);

void printParseTreeTestCase(const ParseTreeNode *node, const Module *program);
