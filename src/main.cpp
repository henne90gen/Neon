#include "AST.h"
#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"

#include <string>

int main() {
    bool verbose = true;
    CodeProvider *codeProvider = new FileCodeProvider("main.ne");
    Lexer lexer(codeProvider, verbose);
    Parser parser(lexer, verbose);

    auto parseTreeRoot = parser.createParseTree();
    if (verbose) {
        printParseTree(parseTreeRoot);
        printParseTreeTestCase(parseTreeRoot);
    }

    auto astRoot = createAstFromParseTree(parseTreeRoot);
    if (verbose) {
        astRoot->print();
    }

    Interpreter interpreter = {};
    interpreter.interpret(astRoot);

    generateIR(astRoot);
    return 0;
}
