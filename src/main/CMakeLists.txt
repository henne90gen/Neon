set(EXECUTABLE_NAME ${PROJECT_NAME})

# run parser generator, when grammar.txt has changed
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Grammar.h ${CMAKE_CURRENT_SOURCE_DIR}/Grammar.cpp
        COMMAND python ${PROJECT_SOURCE_DIR}/scripts/parser_generator.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/grammar.txt)

set(SOURCES_GRAMMAR Grammar.cpp)

set(SOURCES_COMPILER
        Lexer.cpp
        Parser.cpp
        Token.cpp
        Program.cpp
        ast/AstGenerator.cpp
        ast/nodes/AssignmentNode.cpp
        ast/nodes/AstNode.cpp
        ast/nodes/BinaryOperationNode.cpp
        ast/nodes/ForStatementNode.cpp
        ast/nodes/FunctionNode.cpp
        ast/nodes/IfStatementNode.cpp
        ast/nodes/LiteralNodes.cpp
        ast/nodes/SequenceNode.cpp
        ast/nodes/StatementNode.cpp
        ast/nodes/UnaryOperationNode.cpp
        ast/nodes/VariableDefinitionNode.cpp
        ast/nodes/VariableNode.cpp
        ast/visitors/AstInterpreter.cpp
        ast/visitors/AstPrinter.cpp
        ast/visitors/AstTestCasePrinter.cpp
        ast/visitors/AstTypeAnalyser.cpp
        ir/Functions.cpp
        ir/IrGenerator.cpp
        ir/Operations.cpp
        ir/Statements.cpp
        ir/Types.cpp
        ir/Variables.cpp
        Linker.cpp
        ObjectFileWriter.cpp)

add_library(NeonGrammar ${SOURCES_GRAMMAR})
add_library(NeonCompiler ${SOURCES_COMPILER})
add_executable(${EXECUTABLE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_dependencies(NeonCompiler NeonGrammar)

target_link_libraries(NeonCompiler NeonGrammar ${LLVM_LIBS})
target_link_libraries(${EXECUTABLE_NAME} NeonCompiler)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_compile_options(${LIB_NAME} PRIVATE /std:c++17)
else ()
    add_compile_options(${LIB_NAME}
            PRIVATE -Wall
            -Wextra
            -pedantic
            -Wno-unused-parameter)
    add_definitions(-std=c++17)
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif ()

if (CLANG_TIDY_EXE AND RUN_CLANG_TIDY)
    set_target_properties(NeonCompiler PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
endif ()

# TODO precompiled headers are coming in 3.16
# target_precompile_headers(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stdinc.h)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include-pch ${CMAKE_CURRENT_BINARY_DIR}/stdinc.h.pch")
#add_custom_target(pch COMMAND clang -O3 -x c++-header -std=c++17 -std=gnu++17 ${CMAKE_CURRENT_SOURCE_DIR}/stdinc.h -o ${CMAKE_CURRENT_BINARY_DIR}/stdinc.h.pch)
#add_dependencies(${LIB_NAME} pch)
