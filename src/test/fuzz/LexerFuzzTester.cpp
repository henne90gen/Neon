#include "compiler/lexer/Lexer.h"
#include <iterator>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    Logger logger = {};
    auto codeProvider = ByteCodeProvider((char *)data, size);
    Lexer lexer(&codeProvider, logger);
    lexer.getToken();
    return 0;
}
