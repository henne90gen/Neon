cmake_minimum_required(VERSION 3.14)

project(Neon)

# Clang Tidy
message(STATUS "Looking for clang-tidy")
find_program(CLANG_TIDY_EXE
        NAMES "clang-tidy"
        DOC "Path to clang-tidy executable")
if (NOT CLANG_TIDY_EXE)
    message(STATUS "Looking for clang-tidy - not found")
else ()
    message(STATUS "Looking for clang-tidy - found")
    set(
            DO_CLANG_TIDY
            "${CLANG_TIDY_EXE}"
            "-checks=*,-fuchsia-*,-cppcoreguidelines-pro-type-union-access,-hicpp-vararg,-cppcoreguidelines-pro-type-vararg,-cppcoreguidelines-owning-memory,-readability-magic-numbers,-google-runtime-int,-readability-braces-around-statements,-cppcoreguidelines-pro-bounds-constant-array-index,-modernize-use-trailing-return-type"
    )
endif ()

# LLVM
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
        native)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_VERBOSE_MAKEFILE OFF)

enable_testing()
add_subdirectory(src)
