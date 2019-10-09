# Neon

Neon is an experimental programming language.

## Features

### Variables

- declaring new variables
- reading variables
- writing variables

### Functions

- declaring functions
- calling functions
- passing arguments to function calls

### Data Types

- Integer (32 bit)
- Float (32 bit)
- Booleans (1 bit)

### Link external object files

It is possible to link an object file with a program written in Neon,
so that one can call functions that are contained in the externally linked object file.

## TODO / Ideas

- importing files
    - generate function in every module that contains all 'script' code
    - generate function call to that function in place of the import statement
- allow functions to be placed anywhere
    - in global scope
    - in function scope
    - deeply nested in multiple functions, etc.
    - make sure scoping of variables is done right.
    - basically every function is a lambda
- using musl as the runtime library to create smaller binaries
