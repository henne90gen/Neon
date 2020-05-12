#include "Linker.h"
#include "compiler/Compiler.h"

int main() {
    bool verbose = true;
//    auto program = new Program("examples/types.ne");
    auto program = new Program("main.ne");

    auto compiler = Compiler(program, verbose);
    compiler.run();

    auto linker = Linker(program);
    linker.link();

    return 0;
}
