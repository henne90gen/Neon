#pragma once

#include "../BuildEnv.h"
#include "../Program.h"
#include "MetaTypes.h"
#include "ModuleCompileState.h"

class Compiler {
  public:
    Compiler(Program *program, const BuildEnv *buildEnv, const Logger &logger)
        : program(program), buildEnv(buildEnv), log(logger) {}

    bool run();

  private:
    Program *program;
    const BuildEnv *buildEnv;
    const Logger &log;

    std::unordered_map<Module *, ModuleCompileState> moduleCompileState = {};

    Module *loadModule(const std::string &moduleFileName);
    void writeModuleToObjectFile();
    void mergeModules(llvm::Module &destinationModule, const llvm::DataLayout &dataLayout,
                      const std::string &targetTriple);
    void generateIR();
    void analyseTypes();
};
