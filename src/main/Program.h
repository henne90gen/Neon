#pragma once

#include "Token.h"
#include "ast/nodes/AstNode.h"

#include <algorithm>
#include <vector>
#include <string>

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s);

std::string rtrim(const std::string &s);

std::string trim(const std::string &s);

struct Program {
    std::string fileName;
    AstNode *astRoot = nullptr;
    std::vector<Token> tokens = {};

    [[nodiscard]] std::string toString() const;
};
