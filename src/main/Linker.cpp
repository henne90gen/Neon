#include "Linker.h"

#include <iostream>

bool Linker::link() {
    // TODO find out what these options do:
    //  -pie --eh-frame-hdr -m elf_x86_64

    std::string s = "ld";

    // specify which dynamic linker should be used at runtime for dynamically linked libraries
    s += " -dynamic-linker /lib64/ld-linux-x86-64.so.2";

    // specify output file name
    s += " -o " + buildEnv->buildDirectory + program->name;

    // additional object files that have to be linked (they contain the actual entry point and some setup stuff)
    s += " /lib64/crt1.o";
    s += " /lib64/crti.o";
    s += " /lib64/crtn.o";

    // search directory for c standard library and math library
    s += " -L/usr/lib/gcc/x86_64-pc-linux-gnu/9.2.0/";

    // search directory for Neon standard library
    s += " -L" + buildEnv->buildDirectory;

    // specify the object file to link
    s += " " + buildEnv->buildDirectory + program->objectFileName;

    // c: c standard library, m: math library
    s += " -lc -lm";

    // Neon standard library
    s += " -lNeonStd";

    if (verbose) {
        std::cout << "Calling linker with the following command:" << std::endl << s << std::endl;
    }

    // TODO capture stdout and stderr. only print to console when verbose==true
    const char *command = s.c_str();
    int statusCode = system(command);

    if (verbose) {
        std::cout << "Finished linking." << std::endl;
    }
    return statusCode != 0;
}
