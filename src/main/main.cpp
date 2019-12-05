#include "Lexer.h"
#include "Linker.h"
#include "Module.h"
#include "ObjectFileWriter.h"
#include "Parser.h"
#include "ast/AstGenerator.h"
#include "ast/visitors/AstPrinter.h"
#include "ast/visitors/AstTestCasePrinter.h"
#include "ast/visitors/AstTypeAnalyser.h"
#include "ir/IrGenerator.h"

Module *compileModule(Program *program, const std::string &moduleFileName, const bool verbose) {
    auto result = new Module(moduleFileName, program->llvmContext);

    CodeProvider *codeProvider = new FileCodeProvider(result);
    Lexer lexer(codeProvider, result, verbose);
    Parser parser(lexer, result, verbose);

    auto parseTreeRoot = parser.createParseTree();
    if (verbose) {
        printParseTree(parseTreeRoot);
        printParseTreeTestCase(parseTreeRoot, result);
    }

    auto astGenerator = AstGenerator(result);
    astGenerator.run(parseTreeRoot);
    for (auto &importedModule : astGenerator.importedModules) {
        program->uncompiledModules.push_back(importedModule);
    }

    if (verbose) {
        auto astPrinter = AstPrinter(result);
        astPrinter.run();

        auto astTestCasePrinter = AstTestCasePrinter(result);
        astTestCasePrinter.run();
    }

    // TODO fix type analysis
    //    analyseTypes();

    auto generator = IrGenerator(result, verbose);
    generator.run();

    return result;
}

int main() {
    bool verbose = true;
    auto program = new Program("main", {"main.ne"});

    while (!program->uncompiledModules.empty()) {
        std::string moduleFileName = program->uncompiledModules.back();
        program->uncompiledModules.pop_back();
        auto module = compileModule(program, moduleFileName, verbose);
        program->modules[moduleFileName] = module;
    }

    writeModuleToObjectFile(program);

    auto linker = Linker(program);
    linker.link();

    return 0;
}
