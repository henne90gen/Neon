cmake_minimum_required(VERSION 3.14)
project(Neon)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_VERBOSE_MAKEFILE OFF)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "LLVM_INCLUDE_DIRS=${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM_DEFINITIONS=${LLVM_DEFINITIONS}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
# define needed llvm libraries here
llvm_map_components_to_libnames(LLVM_LIBS
                                core
                                support
                                passes
                                mcjit
                                native)

# run parser generator, when grammar.txt has changed
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/Grammar.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/src/Grammar.cpp
                   COMMAND python
                           ${CMAKE_CURRENT_SOURCE_DIR}/parser_generator.py
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/grammar.txt)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Token.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Grammar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ast/ASTInterpreter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ast/ASTPrinter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ast/ASTGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/IRGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ast/ASTTestCasePrinter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Program.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ast/AST.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ast/ASTTypeAnalyser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ObjectFileWriter.cpp)

add_library(${PROJECT_NAME}_lib ${SOURCES})
target_link_libraries(${PROJECT_NAME}_lib ${LLVM_LIBS})

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)

# Tests
enable_testing()
include(${CMAKE_CURRENT_SOURCE_DIR}/vendor/catch/contrib/Catch.cmake)
set(CATCH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/catch/single_include)

set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/LexerTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/parser/ParserTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/parser/FunctionTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/parser/OperationTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/ast/ASTGeneratorTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/test/ast/ASTTestHelper.cpp)

add_executable(Tests ${TEST_SOURCES})
target_include_directories(Tests
                           PRIVATE ${CATCH_INCLUDE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(Tests ${PROJECT_NAME}_lib)
catch_discover_tests(Tests)
