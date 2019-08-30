cmake_minimum_required(VERSION 3.14)
project(MyFirstLanguage)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "LLVM_INCLUDE_DIRS=${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM_DEFINITIONS=${LLVM_DEFINITIONS}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core)

# run parser generator, when grammar.txt has changed
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/Grammar.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/src/Grammar.cpp
                   COMMAND python
                           ${CMAKE_CURRENT_SOURCE_DIR}/parser_generator.py
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/grammar.txt)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Token.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Grammar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AbstractSyntaxTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Interpreter.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${llvm_libs})
