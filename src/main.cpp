#include "AbstractSyntaxTree.h"
#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "Token.h"

#include <iostream>
#include <string>

void printParseTree(ParseTreeNode *root, int indentation = 0) {
  for (int i = 0; i < indentation; i++) {
    std::cout << "  ";
  }
  std::cout << "Node " << to_string(root->symbol) << std::endl;
  for (auto node : root->children) {
    printParseTree(node, indentation + 1);
  }
}

void printAst(AstNode *node, int indentation = 0) {
  if (node == nullptr) {
    std::cout << "Node was a nullptr" << std::endl;
    return;
  }
  for (int i = 0; i < indentation; i++) {
    std::cout << "  ";
  }
  std::cout << "AstNode " << to_string(node->type) << std::endl;
  for (auto child : node->children) {
    printAst(child, indentation + 1);
  }
}

int main() {
  CodeProvider *codeProvider = new FileCodeProvider("main.mfl");
  Lexer lexer = {codeProvider};
  Parser parser = {lexer};

  auto parseTreeRoot = parser.createParseTree();
  // printParseTree(parseTreeRoot);

  auto astRoot = AstNode::createAstFromParseTree(parseTreeRoot);
  // printAst(astRoot);

  Interpreter interpreter = {};
  interpreter.interpret(astRoot);
  return 0;
}
