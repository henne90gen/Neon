#pragma once

#include "compiler/Parser.h"

#include "compiler/ast/AstGenerator.h"
#include "compiler/ast/nodes/AstNode.h"

struct AstNodeSpec {
    int indent;
    ast::NodeType type;
};

struct SimpleTree {
    ast::NodeType type = ast::NodeType::SEQUENCE;
    std::vector<SimpleTree *> children = {};
};

SimpleTree *createSimpleFromAst(AstNode *node);

void assertProgramCreatesAst(const std::vector<std::string> &program, std::vector<AstNodeSpec> &spec);
