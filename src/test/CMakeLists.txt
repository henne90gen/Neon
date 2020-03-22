include(${PROJECT_SOURCE_DIR}/vendor/catch/contrib/Catch.cmake)
set(CATCH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/catch/single_include)

set(SOURCES_TEST
        main.cpp
        LexerTest.cpp
        parser/ParserTest.cpp
        parser/FunctionTest.cpp
        parser/OperationTest.cpp
        ast/AstGeneratorTest.cpp
        ast/AstTestHelper.cpp)

add_executable(Tests ${SOURCES_TEST})
target_include_directories(Tests
        PRIVATE ${CATCH_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/src/main)
target_link_libraries(Tests NeonGrammar NeonCompiler)
catch_discover_tests(Tests)


add_executable(FuzzTests fuzz/FuzzTester.cpp)
target_include_directories(FuzzTests PRIVATE ${PROJECT_SOURCE_DIR}/src/main)
target_compile_options(FuzzTests PRIVATE $<$<C_COMPILER_ID:Clang>:-g -O1 -fsanitize=fuzzer>)
target_link_libraries(FuzzTests PRIVATE $<$<C_COMPILER_ID:Clang>:-fsanitize=fuzzer> NeonGrammar NeonCompiler)

# Allow short runs during automated testing to see if something new breaks
set(FUZZ_RUNTIME 5 CACHE STRING "Number of seconds to run fuzz tests during ctest run")

add_test(NAME RunFuzzTests COMMAND FuzzTests -max_total_time=${FUZZ_RUNTIME})
