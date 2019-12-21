set(EXECUTABLE_NAME ${PROJECT_NAME})

# run parser generator, when grammar.txt has changed
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Grammar.h ${CMAKE_CURRENT_SOURCE_DIR}/Grammar.cpp
        COMMAND python ${PROJECT_SOURCE_DIR}/scripts/parser_generator.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/grammar.txt)

add_library(NeonGrammar Grammar.cpp)

add_library(NeonCompiler
        compiler/ast/AstGenerator.cpp
        compiler/ast/nodes/AssignmentNode.cpp
        compiler/ast/nodes/AstNode.cpp
        compiler/ast/nodes/BinaryOperationNode.cpp
        compiler/ast/nodes/ForStatementNode.cpp
        compiler/ast/nodes/FunctionNode.cpp
        compiler/ast/nodes/IfStatementNode.cpp
        compiler/ast/nodes/ImportNode.cpp
        compiler/ast/nodes/LiteralNodes.cpp
        compiler/ast/nodes/SequenceNode.cpp
        compiler/ast/nodes/StatementNode.cpp
        compiler/ast/nodes/UnaryOperationNode.cpp
        compiler/ast/nodes/VariableDefinitionNode.cpp
        compiler/ast/nodes/VariableNode.cpp
        compiler/ast/visitors/AstInterpreter.cpp
        compiler/ast/visitors/AstPrinter.cpp
        compiler/ast/visitors/AstTestCasePrinter.cpp
        compiler/ast/visitors/TypeAnalyser.cpp
        compiler/ast/visitors/FunctionFinder.cpp
        compiler/ast/visitors/ImportFinder.cpp
        compiler/ir/Functions.cpp
        compiler/ir/IrGenerator.cpp
        compiler/ir/Operations.cpp
        compiler/ir/Statements.cpp
        compiler/ir/Types.cpp
        compiler/ir/Variables.cpp
        compiler/Compiler.cpp
        compiler/FunctionResolver.cpp
        compiler/Lexer.cpp
        compiler/Parser.cpp
        compiler/Token.cpp
        compiler/TypeResolver.cpp
        Linker.cpp
        Module.cpp
        Program.cpp
        Utils.cpp)

add_dependencies(NeonCompiler NeonGrammar)
add_dependencies(NeonCompiler NeonStd)

add_executable(${EXECUTABLE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

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
