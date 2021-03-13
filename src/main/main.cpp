#include <iostream>

#include "BuildEnv.h"
#include "Linker.h"
#include "compiler/Compiler.h"

int main() {
    bool verbose = true;
    //    auto program = new Program("examples/types.ne");
    auto program = new Program("main.ne");
    auto buildEnv = new BuildEnv();

    auto compiler = Compiler(program, buildEnv, verbose);
    if (compiler.run()) {
        std::cout << "Aborting after failed compilation..." << std::endl;
        return 1;
    }

    auto linker = Linker(program, buildEnv, verbose);
    if (linker.link()) {
        return 1;
    }

    return 0;
}
