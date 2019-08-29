#pragma once

#include "Grammar.h"
#include "Lexer.h"

// enum ASTNodeType { BINARY_OPERATION, UNARY_OPERATION, LITERAL };

// struct ASTNode {
//   ASTNodeType type;
//   //   lex::Token token;
//   ASTNode *parent;

//   int childrenCount;
//   ASTNode **children;
// };

// class AbstractSyntaxTree {
// public:
//   void generateIR(){};

// private:
//   ASTNode *root = nullptr;
// };

class ParseTreeNode {
public:
  ParseTreeNode(GrammarSymbol symbol) : symbol(symbol) {}

  GrammarSymbol symbol;
  std::vector<ParseTreeNode*> children = {};
};

class Parser {
public:
  Parser(Lexer lexer) : lexer(lexer) {}

  ParseTreeNode *createParseTree();

private:
  Lexer lexer;

  void executeShift(Token &token, std::vector<int> &states,
                    StateTransition &action,
                    std::vector<ParseTreeNode *> &nodes);
  void executeGoto(std::vector<int> &states, StateTransition &action);
  void executeReduce(std::vector<int> &states, StateTransition &action,
                     std::vector<ParseTreeNode *> &nodes);
};
