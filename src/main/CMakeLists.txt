
add_library(NeonCompiler
        compiler/ast/Types.cpp
        compiler/ast/nodes/AssertNode.cpp
        compiler/ast/nodes/AssignmentNode.cpp
        compiler/ast/nodes/AstNode.cpp
        compiler/ast/nodes/BinaryOperationNode.cpp
        compiler/ast/nodes/CommentNode.cpp
        compiler/ast/nodes/ForStatementNode.cpp
        compiler/ast/nodes/FunctionNode.cpp
        compiler/ast/nodes/IfStatementNode.cpp
        compiler/ast/nodes/ImportNode.cpp
        compiler/ast/nodes/LiteralNodes.cpp
        compiler/ast/nodes/MemberAccessNode.cpp
        compiler/ast/nodes/SequenceNode.cpp
        compiler/ast/nodes/StatementNode.cpp
        compiler/ast/nodes/TypeDeclarationNode.cpp
        compiler/ast/nodes/TypeMemberNode.cpp
        compiler/ast/nodes/UnaryOperationNode.cpp
        compiler/ast/nodes/VariableDefinitionNode.cpp
        compiler/ast/nodes/VariableNode.cpp
        compiler/ast/visitors/AstInterpreter.cpp
        compiler/ast/visitors/AstPrinter.cpp
        compiler/ast/visitors/AstTestCasePrinter.cpp
        compiler/ast/visitors/TypeAnalyzer.cpp
        compiler/ast/visitors/ComplexTypeFinder.cpp
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
        compiler/lexer/Lexer.cpp
        compiler/parser/Parser.cpp
        compiler/parser/Expression.cpp
        compiler/lexer/Token.cpp
        compiler/TypeResolver.cpp
        Linker.cpp
        Module.cpp
        Program.cpp
        Utils.cpp)

add_dependencies(NeonCompiler NeonStd)
target_link_libraries(NeonCompiler PUBLIC ${LLVM_LIBS})
target_include_directories(NeonCompiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set(EXECUTABLE_NAME ${PROJECT_NAME})
add_executable(${EXECUTABLE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

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

if (CLANG_TIDY_EXE AND RUN_CLANG_TIDY)
    set_target_properties(NeonCompiler PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
endif ()
