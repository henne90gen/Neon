#pragma once

#include "Parser.h"

#include "ast/ASTGenerator.h"
#include "ast/nodes/AstNode.h"

struct AstNodeSpec {
    int indentation;
    AstNode::AstNodeType type;
};

struct SimpleTree {
    AstNode::AstNodeType type = AstNode::AstNodeType::SEQUENCE;
    std::vector<SimpleTree *> children = {};
};

SimpleTree *createSimpleFromAst(AstNode *node);

void assertProgramCreatesAst(const std::vector<std::string> &program, std::vector<AstNodeSpec> &spec);
