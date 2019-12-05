#pragma once

#include "../Program.h"

class Compiler {
  public:
    Compiler(Program *program, bool verbose) : program(program), verbose(verbose) {}
    void run();

  private:
    Program *program;
    bool verbose;

    Module *ingestModule(const std::string &moduleFileName);
    void writeModuleToObjectFile();
    void mergeModules(llvm::Module &module, const llvm::DataLayout &dataLayout, const std::string &targetTriple);
    void generateIR();
};
