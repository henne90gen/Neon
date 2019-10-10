#pragma once

#include "ir/IRGenerator.h"
#include "Program.h"

void writeModuleToObjectFile(const Program &program, IRGenerator *irGenerator);
