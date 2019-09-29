#include "IRGenerator.h"
#include "Lexer.h"
#include "Parser.h"
#include "Program.h"
#include "ast/ASTGenerator.h"
#include "ast/ASTPrinter.h"
#include "ast/ASTTestCasePrinter.h"

int main() {
    bool verbose = true;
    Program program = {"main.ne"};
    CodeProvider *codeProvider = new FileCodeProvider(program);
    Lexer lexer(codeProvider, program, verbose);
    Parser parser(lexer, program, verbose);

    auto parseTreeRoot = parser.createParseTree();
    if (verbose) {
        printParseTree(parseTreeRoot);
        printParseTreeTestCase(parseTreeRoot, program);
    }

    auto astRoot = createAstFromParseTree(parseTreeRoot);
    if (verbose) {
        printAst(astRoot);
        printAstTestCase(program, astRoot);
    }

    generateIR(astRoot, program);
    return 0;
}
