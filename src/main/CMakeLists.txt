set(EXECUTABLE_NAME ${PROJECT_NAME})

# run parser generator, when grammar.txt has changed
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Grammar.h ${CMAKE_CURRENT_SOURCE_DIR}/Grammar.cpp
        COMMAND python ${PROJECT_SOURCE_DIR}/scripts/parser_generator.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/grammar.txt)

set(SOURCES_PARSER
        ${CMAKE_CURRENT_SOURCE_DIR}/Grammar.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Lexer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Parser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Token.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Program.cpp)

set(SOURCES_AST_GENERATOR
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/AstGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/nodes/AssignmentNode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/nodes/AstNode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/nodes/BinaryOperationNode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/nodes/ForStatementNode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/nodes/FunctionNode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/nodes/IfStatementNode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/nodes/LiteralNodes.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/nodes/SequenceNode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/nodes/StatementNode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/nodes/UnaryOperationNode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/nodes/VariableDefinitionNode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/nodes/VariableNode.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/visitors/AstInterpreter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/visitors/AstPrinter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/visitors/AstTestCasePrinter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/visitors/AstTypeAnalyser.cpp)

set(SOURCES_IR_GENERATOR
        ${CMAKE_CURRENT_SOURCE_DIR}/ir/Functions.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ir/IrGenerator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ir/Operations.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ir/Statements.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ir/Types.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ir/Variables.cpp)

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Linker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ObjectFileWriter.cpp)

add_library(NeonParser ${SOURCES_PARSER})
add_library(NeonAstGenerator ${SOURCES_AST_GENERATOR})
add_library(NeonIrGenerator ${SOURCES_IR_GENERATOR})
add_library(NeonOther ${SOURCES})
add_executable(${EXECUTABLE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_dependencies(NeonAstGenerator NeonParser)
add_dependencies(NeonIrGenerator NeonParser)
add_dependencies(NeonOther NeonParser)

target_link_libraries(NeonIrGenerator ${LLVM_LIBS})
target_link_libraries(${EXECUTABLE_NAME}
        NeonParser
        NeonAstGenerator
        NeonIrGenerator
        NeonOther)

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
    set_target_properties(${LIB_NAME} PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
endif ()

# TODO precompiled headers are coming in 3.16
# target_precompile_headers(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/stdinc.h)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include-pch ${CMAKE_CURRENT_BINARY_DIR}/stdinc.h.pch")
#add_custom_target(pch COMMAND clang -O3 -x c++-header -std=c++17 -std=gnu++17 ${CMAKE_CURRENT_SOURCE_DIR}/stdinc.h -o ${CMAKE_CURRENT_BINARY_DIR}/stdinc.h.pch)
#add_dependencies(${LIB_NAME} pch)
