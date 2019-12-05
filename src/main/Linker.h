#pragma once

#include "Program.h"

class Linker {
  public:
    explicit Linker(const Program *program) : program(program) {}

    void link();

  private:
    const Program *program;
};
