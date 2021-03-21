#include "Program.h"

#include <iostream>

Program::Program(std::string _entryPoint) : entryPoint(std::move(_entryPoint)) {
    auto position = entryPoint.rfind('.');
    if (position == std::string::npos) {
        std::cerr << "Could not remove extension" << std::endl;
        exit(1);
    }

    name = entryPoint.substr(0, position);
}

std::string Program::objectFileName() const {
#if WIN32
    return name + ".obj";
#else
    return name + ".o";
#endif
}

std::string Program::executableFileName() const {
#if WIN32
    return name + ".exe";
#else
    return name;
#endif
}
