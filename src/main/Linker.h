#pragma once

#include "BuildEnv.h"
#include "Program.h"

#include <cmath>

class Linker {
  public:
    explicit Linker(const Program *program, const BuildEnv *buildEnv, bool verbose = true)
        : program(program), buildEnv(buildEnv), verbose(verbose) {}

    bool link();

  private:
    const Program *program;
    const BuildEnv *buildEnv;
    bool verbose;
};
