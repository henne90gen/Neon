#include "IRGenerator.h"
#include "Lexer.h"
#include "Parser.h"
#include "ast/ASTGenerator.h"
#include "ast/ASTInterpreter.h"
#include "ast/ASTPrinter.h"

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
    if (verbose && astRoot != nullptr) {
        auto printer = new ASTPrinter();
        astRoot->accept(printer);
    }

    interpretAst(astRoot, verbose);

    generateIR(astRoot);
    return 0;
}
