#include "Linker.h"

Linker::Linker(Program &program) : program(program) {}

void Linker::link() {
    std::string s = "ld";
    s += " -dynamic-linker /lib64/ld-linux-x86-64.so.2";
    s += " /lib64/crt1.o";
    s += " /lib64/crti.o";
    s += " /lib64/crtn.o";
    s += " -L/usr/lib/gcc/x86_64-pc-linux-gnu/9.1.0/";
    s += " -lc -lgcc -lgcc_s";
    s += " scripts/link-reverse.o";
    s += " -o t.exe";
    s += " main.ne.o";
    const char *command = s.c_str();
    system(command);
}
