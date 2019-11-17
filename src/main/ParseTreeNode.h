#pragma once

#include "Grammar.h"
#include "Token.h"

struct ParseTreeNode {
    GrammarSymbol symbol;
    Token token;
    std::vector<ParseTreeNode *> children = {};
};
