#pragma once

#include "Token.h"
#include "ast/AST.h"

#include <vector>

struct Program {
    AstNode *astRoot = nullptr;
    std::vector<Token> tokens = {};

    std::string toString() const {
        std::string programStr;
        for (const auto &t : tokens) {
            programStr += t.content + " ";
        }
        return programStr;
    }
};
