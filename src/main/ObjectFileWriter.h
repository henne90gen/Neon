#pragma once

#include "Program.h"
#include "ir/IrGenerator.h"

void writeModuleToObjectFile(const Program &program, IrGenerator *irGenerator);
