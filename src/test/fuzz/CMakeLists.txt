add_executable(LexerFuzzTests LexerFuzzTester.cpp)
target_compile_options(LexerFuzzTests PRIVATE $<$<C_COMPILER_ID:Clang>:-g -O1 -fsanitize=fuzzer>)
target_link_libraries(LexerFuzzTests PRIVATE $<$<C_COMPILER_ID:Clang>:-fsanitize=fuzzer,memory> NeonCompiler)

add_executable(ParserFuzzTests ParserFuzzTester.cpp)
target_compile_options(ParserFuzzTests PRIVATE $<$<C_COMPILER_ID:Clang>:-g -O1 -fsanitize=fuzzer>)
target_link_libraries(ParserFuzzTests PRIVATE $<$<C_COMPILER_ID:Clang>:-fsanitize=fuzzer,memory> NeonCompiler)
