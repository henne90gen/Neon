#include "Compiler.h"

#include "FunctionResolver.h"
#include "ast/AstGenerator.h"
#include "ast/visitors/AstPrinter.h"
#include "ast/visitors/AstTestCasePrinter.h"
#include "ast/visitors/FunctionFinder.h"
#include "ast/visitors/ImportFinder.h"
#include "ast/visitors/TypeAnalyser.h"
#include "ir/IrGenerator.h"

#include <iostream>
#include <string>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Transforms/Utils/Cloning.h>

void Compiler::run() {
    std::string entryPoint = std::filesystem::absolute(std::filesystem::path(program->entryPoint)).string();
    std::vector<std::string> uncompiledModules = {entryPoint};
    while (!uncompiledModules.empty()) {
        std::string moduleFileName = uncompiledModules.back();
        uncompiledModules.pop_back();

        auto itr = program->modules.find(moduleFileName);
        bool moduleAlreadyExists = itr != program->modules.end();
        if (moduleAlreadyExists) {
            std::cout << "Skipping " << moduleFileName << std::endl;
            continue;
        }

        auto module = loadModule(moduleFileName);
        program->modules[moduleFileName] = module;

        for (auto &importedModule : moduleImportsMap[module]) {
            uncompiledModules.push_back(importedModule);
        }
    }

    analyseTypes();

    generateIR();

    writeModuleToObjectFile();

    std::cout << "Finished compilation." << std::endl;
}

Module *Compiler::loadModule(const std::string &moduleFileName) {
    auto module = new Module(moduleFileName, program->llvmContext);

    Lexer lexer(module->getCodeProvider(), verbose);
    Parser parser(lexer, module->tokens, verbose);

    auto parseTreeRoot = parser.createParseTree();
    if (verbose) {
        printParseTree(parseTreeRoot);
        printParseTreeTestCase(parseTreeRoot, module->toArrayString());
    }

    auto astGenerator = AstGenerator(module);
    astGenerator.run(parseTreeRoot);

    if (module->root == nullptr) {
        std::cerr << "Could not create AST" << std::endl;
        return module;
    }

    if (verbose) {
        auto astPrinter = AstPrinter(module);
        astPrinter.run();

        auto astTestCasePrinter = AstTestCasePrinter(module);
        astTestCasePrinter.run();
    }

    moduleImportsMap[module] = ImportFinder(module->getDirectoryPath()).run(module->root);
    moduleFunctionsMap[module] = FunctionFinder().run(module->root);

    return module;
}

void Compiler::generateIR() {
    auto functionResolver = FunctionResolver(program, moduleImportsMap, moduleFunctionsMap);
    for (const auto &module : program->modules) {
        auto typeResolver = TypeResolver(moduleTypesMap[module.second]);
        auto generator = IrGenerator(module.second, functionResolver, typeResolver, verbose);
        generator.run();
    }
}

// void IrGenerator::generateDummyMain() {
//    if (llvmModule.getFunction("main") != nullptr) {
//        return;
//    }
//
//    auto zero = new IntegerNode(0);
//
//    auto returnStatement = new StatementNode();
//    returnStatement->setIsReturnStatement(true);
//    returnStatement->setChild(zero);
//
//    auto function = new FunctionNode("main" + module->fileName, ast::DataType::INT);
//    function->setBody(returnStatement);
//
//    visitFunctionNode(function);
//}

void Compiler::mergeModules(llvm::Module &destinationModule, const llvm::DataLayout &dataLayout,
                            const std::string &targetTriple) {
    // To be able to link modules, they have to be in the same context.
    // To create modules in parallel, they can't share a context.
    // One can work around this issue, by writing all the modules into a buffer first and then reading them in again
    // with the correct context

    for (auto &mod : program->modules) {
        llvm::Module &llvmModule = mod.second->llvmModule;
        llvmModule.setDataLayout(dataLayout);
        llvmModule.setTargetTriple(targetTriple);

        auto error = llvm::Linker::linkModules(destinationModule, llvm::CloneModule(llvmModule));
        if (error) {
            std::cerr << "Could not link modules" << std::endl;
            exit(1);
        }
    }
}

void Compiler::writeModuleToObjectFile() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    std::string Error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, Error);

    if (target == nullptr) {
        llvm::errs() << Error;
        exit(1);
    }

    auto cpu = "generic";
    auto features = "";
    llvm::TargetOptions targetOptions = {};
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, targetOptions, RM);
    auto dataLayout = targetMachine->createDataLayout();

    auto module = llvm::Module(program->objectFileName, program->llvmContext);
    module.setDataLayout(dataLayout);
    module.setTargetTriple(targetTriple);

    mergeModules(module, dataLayout, targetTriple);

    // TODO generate dummy main function, to enable users to write "scripts"

    if (llvm::verifyModule(module, &llvm::errs())) {
        exit(1);
    }

    // print llvm ir to console
    module.print(llvm::outs(), nullptr);

    // write object file
    std::error_code EC;
    llvm::raw_fd_ostream dest(program->objectFileName, EC, llvm::sys::fs::OF_None);

    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        exit(1);
    }

    auto fileType = llvm::TargetMachine::CGFT_ObjectFile;
    llvm::legacy::PassManager pass;
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
        llvm::errs() << "TargetMachine can't emit a file of this type";
        exit(1);
    }

    pass.run(module);
    dest.flush();
    dest.close();
}

void Compiler::analyseTypes() {
    for (auto &entry : program->modules) {
        auto &module = entry.second;
        auto functionResolver = FunctionResolver(program, moduleImportsMap, moduleFunctionsMap);
        moduleTypesMap[module] = TypeAnalyser(module, functionResolver).run(module->root);
    }
}
