#include "Linker.h"

#include <iostream>

void Linker::link() {
    std::string s = "ld";
    s += " -dynamic-linker /lib64/ld-linux-x86-64.so.2";
    s += " /lib64/crt1.o";
    s += " /lib64/crti.o";
    s += " /lib64/crtn.o";
    s += " -L/usr/lib/gcc/x86_64-pc-linux-gnu/9.2.0/";
    s += " -L./";
    s += " -lc -lgcc -lgcc_s -lm -lNeonLib";

    s += " " + program->objectFileName;
    s += " -o " + program->name;

    std::cout << std::endl << "Calling linker with the following command:" << std::endl << s << std::endl;

    const char *command = s.c_str();
    system(command);
}
