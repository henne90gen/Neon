#include "AbstractSyntaxTree.h"
#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "Token.h"

#include <iostream>
#include <string>

int main() {
    bool verbose = true;
    CodeProvider *codeProvider = new FileCodeProvider("main.mfl");
    Lexer lexer(codeProvider, verbose);
    Parser parser(lexer, verbose);

    auto parseTreeRoot = parser.createParseTree();
    if (verbose) {
        printParseTree(parseTreeRoot);
    }

    auto astRoot = AstNode::createAstFromParseTree(parseTreeRoot);
    if (verbose) {
        //        printAst(astRoot);
    }

    Interpreter interpreter = {};
    interpreter.interpret(astRoot);
    return 0;
}
