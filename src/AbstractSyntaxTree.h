#pragma once

#include "Parser.h"

class FloatData {
public:
    FloatData(float value) : value(value) {}

    float value;
};

class IntegerData {
public:
    IntegerData(int value) : value(value) {}

    int value;
};

class BoolData {
public:
    BoolData(int value) : value(value) {}

    bool value;
};

class AstNode {
public:
    enum AstNodeType {
        BIN_OP_ADD,
        BIN_OP_MUL,
        BIN_OP_SUB,
        BIN_OP_DIV,
        UNARY_OP_NOT,
        SEQUENCE,
        STATEMENT,
        INT_LIT,
        FLOAT_LIT,
        BOOL_LIT
    };

    static AstNode *createAstFromParseTree(ParseTreeNode *node);

    AstNode(AstNodeType type) : type(type) {}

    AstNodeType type;
    std::vector<AstNode *> children = {};
    void *data;
};

std::string to_string(AstNode::AstNodeType type);
