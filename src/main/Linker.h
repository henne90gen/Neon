#pragma once

#include "Program.h"

#include <vector>

class Linker {
  public:
    explicit Linker(std::string programName, Program &program);

    void link();

  private:
    std::string programName;
    Program &program;
};
