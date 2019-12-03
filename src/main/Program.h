#pragma once

#include "Token.h"
#include "ast/nodes/AstNode.h"

#include <algorithm>
#include <string>
#include <vector>

struct Program {
    std::string fileName;
    AstNode *astRoot = nullptr;
    std::vector<Token> tokens = {};

    [[nodiscard]] std::string toString() const;
    std::string toEscapedString()const;
    std::string toArrayString()const;
};
