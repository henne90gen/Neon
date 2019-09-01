#pragma once

#include "Grammar.h"
#include "Lexer.h"

class ParseTreeNode {
public:
    ParseTreeNode(GrammarSymbol symbol) : symbol(symbol) {}

    ParseTreeNode(GrammarSymbol symbol, Token token)
            : symbol(symbol), token(token) {}

    GrammarSymbol symbol;
    Token token;
    std::vector<ParseTreeNode *> children = {};
};

class Parser {
public:
    Parser(Lexer lexer, bool verbose = false) : lexer(lexer), verbose(verbose) {}

    ParseTreeNode *createParseTree();

private:
    Lexer lexer;
    bool verbose;

    void executeShift(Token &token, std::vector<int> &states,
                      StateTransition &action,
                      std::vector<ParseTreeNode *> &nodes);

    void executeGoto(std::vector<int> &states, StateTransition &action);

    void executeReduce(std::vector<int> &states, StateTransition &action,
                       std::vector<ParseTreeNode *> &nodes);

    std::optional<StateTransition> getNextAction(int rowIndex, int columnIndex);
};
