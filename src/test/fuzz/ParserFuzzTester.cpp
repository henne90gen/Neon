#include "../../main/compiler/Logger.h"
#include <compiler/lexer/Lexer.h>
#include <compiler/parser/Parser.h>
#include <iterator>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    Logger logger = {};
    logger.setLogLevel(Logger::DISABLED);
    auto codeProvider = ByteCodeProvider((char *)data, size);
    Lexer lexer(&codeProvider, logger);
    auto context = new llvm::LLVMContext();
    auto module = new Module("", *context);
    Parser parser(logger, lexer);
    parser.run(module);
    return 0;
}
