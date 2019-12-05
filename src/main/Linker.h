#pragma once

#include "Module.h"

#include <vector>

class Linker {
  public:
    explicit Linker(const Program *program) : program(program) {}

    void link();

  private:
    const Program *program;
};
