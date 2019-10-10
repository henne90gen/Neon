set(EXECUTABLE_NAME ${PROJECT_NAME})
set(LIB_NAME ${EXECUTABLE_NAME}_lib)

# run parser generator, when grammar.txt has changed
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Grammar.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/Grammar.cpp
                   COMMAND python ${PROJECT_SOURCE_DIR}/parser_generator.py
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/grammar.txt)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Token.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Grammar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/ASTInterpreter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/ASTPrinter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/ASTGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IRGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/ASTTestCasePrinter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Program.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/AST.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ast/ASTTypeAnalyser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ObjectFileWriter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Linker.cpp)

add_library(${LIB_NAME} ${SOURCES})
target_link_libraries(${LIB_NAME} ${LLVM_LIBS})

add_executable(${EXECUTABLE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(${EXECUTABLE_NAME} ${LIB_NAME})

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_compile_options(${LIB_NAME} PRIVATE /std:c++17)
else()
  target_compile_options(${LIB_NAME}
                         PRIVATE -Wall
                                 -Wextra
                                 -pedantic
                                 -Wno-unused-parameter)
  add_definitions(-std=c++17)
endif()

set_target_properties(${LIB_NAME}
                      PROPERTIES CXX_STANDARD
                                 17
                                 CXX_STANDARD_REQUIRED
                                 ON)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  add_compile_definitions(DEBUG)
endif()

if(CLANG_TIDY_EXE AND RUN_CLANG_TIDY)
  set_target_properties(${LIB_NAME} PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
endif()

# TODO precompiled headers are coming in 3.16
# target_precompile_headers(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stdinc.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include-pch ${CMAKE_CURRENT_BINARY_DIR}/stdinc.h.pch")
add_custom_target(pch COMMAND clang -O3 -x c++-header -std=c++17 -std=gnu++17 ${CMAKE_CURRENT_SOURCE_DIR}/stdinc.h -o ${CMAKE_CURRENT_BINARY_DIR}/stdinc.h.pch)
add_dependencies(${LIB_NAME} pch)
