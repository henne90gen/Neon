#pragma once

#include "../../Module.h"
#include "../ast/nodes/AllNodes.h"

class SimpleParser {
  public:
    SimpleParser(Lexer &lexer, Module *module, bool verbose) : lexer(lexer), module(module), verbose(verbose) {}

    void run();

  private:
    Lexer &lexer;
    Module *module;
    bool verbose;

    Token getNextToken();
    AstNode* parseImportStatement(const Token &token);
};
