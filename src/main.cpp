#include "Lexer.h"
#include "Parser.h"
#include "Token.h"

#include <iostream>
#include <string>

int main() {
  CodeProvider *codeProvider = new FileCodeProvider("main.mfl");
  Lexer lexer = {codeProvider};
  Parser parser = {lexer};
  auto parseTreeRoot = parser.createParseTree();
  return 0;
}
