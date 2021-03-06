#pragma once

#include <string>
#include <vector>

#include "compiler/ast/AstNode.h"

struct AstNodeSpec {
    int indent;
    ast::NodeType type;
};

struct SimpleTree {
    ast::NodeType type = ast::NodeType::SEQUENCE;
    std::vector<SimpleTree *> children = {};
};

SimpleTree *createSimpleFromAst(AstNode *node);

bool parserCreatesCorrectAst(const std::vector<std::string> &program, std::vector<AstNodeSpec> &spec);
