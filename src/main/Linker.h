#pragma once

#include "Program.h"

#include <math.h>

class Linker {
  public:
    explicit Linker(const Program *program) : program(program) {}

    bool link();

  private:
    const Program *program;
};
