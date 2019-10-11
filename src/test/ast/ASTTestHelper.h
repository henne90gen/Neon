#pragma once

#include "Parser.h"

#include "ast/ASTGenerator.h"
#include "ast/nodes/AstNode.h"

struct AstNodeSpec {
    int indentation;
    ast::NodeType type;
};

struct SimpleTree {
    ast::NodeType type = ast::NodeType::SEQUENCE;
    std::vector<SimpleTree *> children = {};
};

SimpleTree *createSimpleFromAst(AstNode *node);

void assertProgramCreatesAst(const std::vector<std::string> &program, std::vector<AstNodeSpec> &spec);
