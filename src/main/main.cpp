#include <iostream>

#include "Linker.h"
#include "compiler/Compiler.h"

int main() {
    bool verbose = true;
    //    auto program = new Program("examples/types.ne");
    auto program = new Program("main.ne");

    auto compiler = Compiler(program, verbose);
    if (compiler.run()) {
        std::cout << "Aborting after failed compilation..." << std::endl;
        return 1;
    }

    auto linker = Linker(program);
    if (linker.link()) {
        return 1;
    }

    return 0;
}
