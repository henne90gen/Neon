#include "Lexer.h"
#include "Linker.h"
#include "ObjectFileWriter.h"
#include "Parser.h"
#include "Program.h"
#include "ast/AstGenerator.h"
#include "ast/visitors/AstPrinter.h"
#include "ast/visitors/AstTestCasePrinter.h"
#include "ast/visitors/AstTypeAnalyser.h"
#include "ir/IrGenerator.h"

void compileProgram(Program &program, const bool verbose) {
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

    auto generator = new IrGenerator(program, verbose);
    generator->run(astRoot);

    writeModuleToObjectFile(program, generator);
}

int main() {
    bool verbose = true;
    Program program = {"main.ne"};

    compileProgram(program, verbose);

    auto linker = new Linker("main.ne.exe", program);
    linker->link();

    return 0;
}
