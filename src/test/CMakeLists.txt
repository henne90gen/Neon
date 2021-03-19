include(${PROJECT_SOURCE_DIR}/vendor/catch/contrib/Catch.cmake)
set(CATCH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/catch/single_include)

# Unit Tests
add_executable(Tests
        main.cpp
        LexerTest.cpp
        parser/FunctionTest.cpp
        parser/OperationTest.cpp
        parser/StatementTest.cpp
        parser/TypesTest.cpp
        parser/ParserTest.cpp
        parser/ParserTestHelper.cpp)
target_include_directories(Tests
        PRIVATE ${CATCH_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/src/main)
target_link_libraries(Tests NeonCompiler)
catch_discover_tests(Tests)

# Fuzz Tests
add_executable(FuzzTests fuzz/LexerFuzzTester.cpp)
target_compile_options(FuzzTests PRIVATE $<$<C_COMPILER_ID:Clang>:-g -O1 -fsanitize=fuzzer>)
target_link_libraries(FuzzTests PRIVATE $<$<C_COMPILER_ID:Clang>:-fsanitize=fuzzer> NeonCompiler)

# Allow short runs during automated testing to see if something new breaks
set(FUZZ_RUNTIME 5 CACHE STRING "Number of seconds to run fuzz tests during ctest run")

add_test(NAME RunFuzzTests COMMAND FuzzTests -max_total_time=${FUZZ_RUNTIME})
