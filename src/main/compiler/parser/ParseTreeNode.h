#pragma once

#include "../lexer/Token.h"
#include "Grammar.h"

struct ParseTreeNode {
    GrammarSymbol symbol;
    Token token;
    std::vector<ParseTreeNode *> children = {};
};
