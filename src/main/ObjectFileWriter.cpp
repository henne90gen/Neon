#include "ObjectFileWriter.h"

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

//void IrGenerator::generateDummyMain() {
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

void mergeModules(Program *program, llvm::Module &module, const llvm::DataLayout &dataLayout,
                  const std::string &targetTriple) {
    // To be able to link modules, they have to be in the same context.
    // To create modules in parallel, they can't share a context.
    // One can work around this issue, by writing all the modules into a buffer first and then reading them in again
    // with the correct context

    for (auto &mod : program->modules) {
        llvm::Module &llvmModule = mod.second->llvmModule;
        llvmModule.setDataLayout(dataLayout);
        llvmModule.setTargetTriple(targetTriple);

        auto error = llvm::Linker::linkModules(module, llvm::CloneModule(llvmModule));
        if (error) {
            std::cerr << "Could not link modules" << std::endl;
            exit(1);
        }
    }
}

void writeModuleToObjectFile(Program *program) {
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

    mergeModules(program, module, dataLayout, targetTriple);

    // TODO generate dummy main function, to enable users to write "scripts"

    if (llvm::verifyModule(module, &llvm::errs())) {
        exit(1);
    }

    module.print(llvm::outs(), nullptr);

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
