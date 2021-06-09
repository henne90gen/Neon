cmake_minimum_required(VERSION 3.14)

project(Neon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(NEON_BUILD_DIR ${PROJECT_SOURCE_DIR}/neon-build/)

# Options
option(RUN_CLANG_TIDY "Compile all code using clang-tidy" OFF)
option(USE_ADDRESS_SANITIZER "Compile all code with address sanitization enabled" OFF)

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
            "${CLANG_TIDY_EXE}" "-fix"
            "-checks=*,-fuchsia-*,-llvmlibc-*,-cppcoreguidelines-pro-type-union-access,-hicpp-vararg,-cppcoreguidelines-pro-type-vararg,-cppcoreguidelines-owning-memory,-readability-magic-numbers,-google-runtime-int,-readability-braces-around-statements,-cppcoreguidelines-pro-bounds-constant-array-index,-modernize-use-trailing-return-type"
    )
endif ()

if (USE_ADDRESS_SANITIZER)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fsanitize=address)
    message("-- Compiling with address sanitizer")
endif ()

# LLVM
find_package(LLVM 12.0.0 REQUIRED CONFIG)

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

if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows"))
    include(ExternalProject)
    ExternalProject_Add(musl
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/musl
            CONFIGURE_COMMAND ${CMAKE_SOURCE_DIR}/vendor/musl/configure --prefix=${CMAKE_SOURCE_DIR}/neon-build/crt --syslibdir=${CMAKE_SOURCE_DIR}/neon-build/crt
            BUILD_COMMAND make -j 8)
endif ()

enable_testing()
add_subdirectory(src)
add_subdirectory(scripts)
