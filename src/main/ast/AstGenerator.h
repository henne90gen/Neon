#pragma once

#include "../Parser.h"
#include "nodes/AstNode.h"

AstNode *createAstFromParseTree(ParseTreeNode *node);
