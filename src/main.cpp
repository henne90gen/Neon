#include "IRGenerator.h"
#include "Lexer.h"
#include "ObjectFileWriter.h"
#include "Parser.h"
#include "Program.h"
#include "ast/ASTGenerator.h"
#include "ast/ASTPrinter.h"
#include "ast/ASTTestCasePrinter.h"
#include "ast/ASTTypeAnalyser.h"

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

    analyseTypes(astRoot);

    auto generator = new IRGenerator(program);
    generator->run(astRoot);

    writeModuleToObjectFile(program, generator);

    return 0;
}
