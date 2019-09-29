#include "IRGenerator.h"
#include "Lexer.h"
#include "Parser.h"
#include "Program.h"
#include "ast/ASTGenerator.h"
#include "ast/ASTInterpreter.h"
#include "ast/ASTPrinter.h"
#include "ast/ASTTestCasePrinter.h"

int main() {
    bool verbose = true;
    CodeProvider *codeProvider = new FileCodeProvider("main.ne");
    Program program = {};
    Lexer lexer(codeProvider, program, verbose);
    Parser parser(lexer, program, verbose);

    auto parseTreeRoot = parser.createParseTree();
    if (verbose) {
        printParseTree(parseTreeRoot);
        //        printParseTreeTestCase(parseTreeRoot, program);
    }

    auto astRoot = createAstFromParseTree(parseTreeRoot);
    if (verbose && astRoot != nullptr) {
        auto printer = new ASTPrinter();
        astRoot->accept(printer);

        printAstTestCase(program, astRoot);
    }

//    interpretAst(astRoot, verbose);

    generateIR(astRoot);
    return 0;
}
