#pragma once

#include <vector>

#include "../lexer/Lexer.h"
#include "Grammar.h"
#include "ParseTreeNode.h"

class Parser {
  public:
    explicit Parser(Lexer &lexer, std::vector<Token> &tokens, bool verbose = false)
        : lexer(lexer), tokens(tokens), verbose(verbose) {}

    ParseTreeNode *createParseTree();

  private:
    Lexer &lexer;
    std::vector<Token> &tokens;
    bool verbose;

    Token getNextToken();

    void executeShift(Token &token, std::vector<int> &states, StateTransition &action,
                      std::vector<ParseTreeNode *> &nodes);

    static void executeGoto(std::vector<int> &states, StateTransition &action);

    void executeReduce(std::vector<int> &states, StateTransition &action, std::vector<ParseTreeNode *> &nodes);

    std::optional<StateTransition> getNextAction(int rowIndex, int columnIndex);
};

void printParseTree(ParseTreeNode *node, int indentation = 0);

void printParseTreeTestCase(const ParseTreeNode *node, const std::string &programAsArrayString);
