#include "Program.h"

#include <iostream>

Program::Program(std::string _entryPoint) : entryPoint(std::move(_entryPoint)) {
    auto position = entryPoint.rfind('.');
    if (position == std::string::npos) {
        std::cerr << "Could not remove extension" << std::endl;
        exit(1);
    }

    name = entryPoint.substr(0, position);
    objectFileName = name + ".o";
}
