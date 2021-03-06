# Neon

Neon is an experimental programming language. It has C-like syntax and is easily interoperable with C/C++.

## Features

### Variables

- declaring new variables: `int i = 0`
- writing variables: `i = 5`
- reading variables: `i = i + 5`
- variable scoping
    - variables that are defined inside of a scope are not accessible from outside that scope
    - variables that are overriden in a scope are available again after that scope

### Functions

- declaring functions: `fun hello(float f) int { … }`
- calling functions: `hello(3.14)`

### Data Types

- Integer (64 bit): `int i = 0`
- Float (64 bit): `float f = 0.0`
- Boolean (1 bit): `bool b = true`
- Array
    - one dimensional arrays of the following primitive data types are supported:
        - `int[5] a`
        - `float[5] a`
        - `bool[5] a`
- String `string s = "Hello World!"`
    - the string type is managed, which means that the length of the string is saved along the data

### Linking external object files

It is possible to link an object file with a program written in Neon. This makes it possible to call functions that are
contained in the externally linked object file.

### Control flow

#### If statements

- if statements work just like in C
- if/else statements work just like in C
- TODO if/else-if/…/else

#### For loops

- for loops work just like in C
- e.g. `for int i = 0; i < 5; i = i + 1 { … }`

### Imports

- other Neon-files can be imported with the `import` keyword
- the imported file is specified with a relative path to the current source file
- e.g. `import "examples/functions.ne"`

### Standard Library

- consists of two parts:
    - functions written in Neon
        - provides definitions to interface with existing c functions
        - for example maths functions
        - still WIP
    - functions written in C
        - provides functions that do not exist in available C libraries
        - for example conversion functions between the primitive data types

### Complex Types

- complex types are types that are built up from other types
- for now only primitive types (int, float, bool, string) are allowed inside complex types
- can be declared like so:

```
type MyType {
   int i
}
```

## TODO / Ideas

- complex types
    - arrays (multidimensional)
    - compound types (structs)
- think about interface definitions
    - how to create them
    - how to implement them
    - how to match types that implement the same interface
    - maybe a mixture between Go and Java?
- add comments (# or //)
    - single line
    - multi line
- importing files
    - place imported functions in a different namespace (the generated function names), so that they don't collide with
      locally defined functions
    - generate function in every module that contains all 'script' code
    - generate function call to that function in place of the import statement
- allow functions to be placed anywhere
    - in global scope
    - in function scope
    - deeply nested in multiple functions, etc.
    - make sure scoping of variables is done right.
    - basically every function is a lambda
- use musl as the runtime library to create smaller binaries
- add reflection capabilities
    - inspect objects at runtime
    - get information about types

## Building LLVM to use with Neon

```bash
cmake -G Ninja -D CMAKE_INSTALL_PREFIX=C:/llvm -D LLVM_TARGETS_TO_BUILD="X86;AArch64" ..
cmake --build . --target install
```
