#pragma once

#include "BuildEnv.h"
#include "Program.h"

#include <cmath>

class Linker {
  public:
    explicit Linker(const Program *program, const BuildEnv *buildEnv) : program(program), buildEnv(buildEnv) {}

    bool link();

  private:
    const Program *program;
    const BuildEnv *buildEnv;
};
