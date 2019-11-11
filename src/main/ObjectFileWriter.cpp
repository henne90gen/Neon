#include "ObjectFileWriter.h"

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>
#include <string>

#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>

void writeModuleToObjectFile(const Program &program, IRGenerator *irGenerator) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();

    const std::string fileName = program.fileName + ".o";
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

    auto &module = irGenerator->getModule();
    module.setDataLayout(targetMachine->createDataLayout());
    module.setTargetTriple(targetTriple);

    if (llvm::verifyModule(module, &llvm::errs())) {
        exit(1);
    }

    std::error_code EC;
    llvm::raw_fd_ostream dest(fileName, EC, llvm::sys::fs::OF_None);

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
}
