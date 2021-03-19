#include "Compiler.h"

#include "FunctionResolver.h"
#include "ast/visitors/AstPrinter.h"
#include "ast/visitors/AstTestCasePrinter.h"
#include "ast/visitors/ComplexTypeFinder.h"
#include "ast/visitors/FunctionFinder.h"
#include "ast/visitors/ImportFinder.h"
#include "ast/visitors/TypeAnalyzer.h"
#include "ir/IrGenerator.h"
#include "parser/Parser.h"

#include <iostream>
#include <string>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Transforms/Utils/Cloning.h>

bool Compiler::run() {
    std::vector<std::string> uncompiledModulePaths = {program->entryPoint};
    while (!uncompiledModulePaths.empty()) {
        std::string moduleFileName = uncompiledModulePaths.back();
        uncompiledModulePaths.pop_back();

        auto itr = program->modules.find(moduleFileName);
        bool moduleAlreadyExists = itr != program->modules.end();
        if (moduleAlreadyExists) {
            log.debug("Skipping " + moduleFileName + " because it has already been processed");
            continue;
        }

        auto module = loadModule(moduleFileName);
        if (module->root == nullptr) {
            log.error("Failed to compile module " + moduleFileName);
            return true;
        }
        program->modules[moduleFileName] = module;

        for (auto &importedModule : moduleCompileState[module].imports) {
            uncompiledModulePaths.push_back(importedModule);
        }
    }

    analyseTypes();

    generateIR();

    writeModuleToObjectFile();

    log.debug("Finished compilation.");
    return false;
}

Module *Compiler::loadModule(const std::string &moduleFileName) {
    auto module = new Module(moduleFileName, program->llvmContext);

    if (module->getFilePath().has_parent_path()) {
        const auto moduleBuildDir =
              std::filesystem::path(buildEnv->buildDirectory) / module->getFilePath().parent_path();
        std::filesystem::create_directories(moduleBuildDir);
    }

    Lexer lexer(module->getCodeProvider(), log);

    Parser parser(lexer, module, log);
    parser.run();

    if (module->root == nullptr) {
        log.error("Could not parse '" + moduleFileName + "'");
        return module;
    }

    if (log.getLogLevel() == Logger::LogLevel::DEBUG_) {
        auto astPrinter = AstPrinter(module);
        astPrinter.run();

        auto astTestCasePrinter = AstTestCasePrinter(module);
        astTestCasePrinter.run();
    }

    moduleCompileState[module].imports = ImportFinder(module->getDirectoryPath()).run(module->root);
    moduleCompileState[module].functions = FunctionFinder().run(module->root);
    moduleCompileState[module].compositeTypes = ComplexTypeFinder().run(module->root);

    return module;
}

void Compiler::generateIR() {
    auto functionResolver = FunctionResolver(program, moduleCompileState);
    for (const auto &entry : program->modules) {
        auto module = entry.second;
        auto typeResolver = TypeResolver(program, moduleCompileState);
        auto generator = IrGenerator(buildEnv, module, functionResolver, typeResolver, log);
        generator.run();
    }
}

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

    auto module = llvm::Module(buildEnv->buildDirectory + program->objectFileName, program->llvmContext);
    module.setDataLayout(dataLayout);
    module.setTargetTriple(targetTriple);

    mergeModules(module, dataLayout, targetTriple);

    if (llvm::verifyModule(module, &llvm::errs())) {
        exit(1);
    }

    // print llvm ir to console
    if (log.getLogLevel() == Logger::LogLevel::DEBUG_) {
        module.print(llvm::outs(), nullptr);
    }

    // print llvm ir to file
    {
        std::error_code EC;
        const std::string filePath = buildEnv->buildDirectory + program->name + ".llvm";
        llvm::raw_fd_ostream destIR(filePath, EC, llvm::sys::fs::OF_None);
        module.print(destIR, nullptr);
        if (EC) {
            llvm::errs() << "Failed to open file (" << filePath << "): " << EC.message() << "\n";
            exit(1);
        }
    }

    // write object file
    {
        std::error_code EC;
        llvm::raw_fd_ostream dest(buildEnv->buildDirectory + program->objectFileName, EC, llvm::sys::fs::OF_None);

        if (EC) {
            llvm::errs() << "Could not open file: " << EC.message() << "\n";
            exit(1);
        }

        auto fileType = llvm::CGFT_ObjectFile;
        llvm::legacy::PassManager pass;
        if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
            llvm::errs() << "TargetMachine can't emit a file of this type";
            exit(1);
        }

        pass.run(module);
        dest.flush();
        dest.close();
    }
}

void Compiler::analyseTypes() {
    for (auto &entry : program->modules) {
        auto &module = entry.second;
        auto functionResolver = FunctionResolver(program, moduleCompileState);
        auto result = TypeAnalyzer(module, functionResolver).run(module->root);
        moduleCompileState[module].nodeToTypeMap = result.first;
        moduleCompileState[module].nameToTypeMap = result.second;
    }
}
