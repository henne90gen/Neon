#pragma once

#include "AST.h"
#include "../Parser.h"

AstNode *createAstFromParseTree(ParseTreeNode *node);
