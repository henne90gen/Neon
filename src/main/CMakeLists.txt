
add_library(NeonCompiler
        compiler/ast/AST.cpp
        compiler/ast/AstNode.cpp
        compiler/ast/Types.cpp
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
        compiler/lexer/Lexer.cpp
        compiler/parser/ControlFlow.cpp
        compiler/parser/Expressions.cpp
        compiler/parser/Functions.cpp
        compiler/parser/Parser.cpp
        compiler/parser/Types.cpp
        compiler/lexer/Token.cpp
        compiler/Compiler.cpp
        compiler/FunctionResolver.cpp
        compiler/Logger.cpp
        compiler/TypeResolver.cpp
        util/Timing.cpp
        Linker.cpp
        Module.cpp
        Program.cpp
        util/Utils.cpp)

add_dependencies(NeonCompiler NeonStd)
if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
    add_dependencies(NeonCompiler musl)
endif ()
target_link_libraries(NeonCompiler PUBLIC ${LLVM_LIBS})
target_include_directories(NeonCompiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Neon ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_link_libraries(Neon NeonCompiler)

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
