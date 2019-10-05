#pragma once

#include "Program.h"

class Linker {
  public:
    explicit Linker(Program &program);

    void link();
  private:
    Program &program;
};
