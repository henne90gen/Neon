#pragma once

#include "BuildEnv.h"
#include "Program.h"

#include "compiler/Logger.h"
#include <cmath>

class Linker {
  public:
    explicit Linker(const Program *program, const BuildEnv *buildEnv, const Logger &logger)
        : program(program), buildEnv(buildEnv), log(logger) {}

    bool link();

  private:
    const Program *program;
    const BuildEnv *buildEnv;
    const Logger &log;

    std::string getLinkerCommand();
};
