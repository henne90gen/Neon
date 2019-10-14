# Neon

Neon is an experimental programming language.
It has C-like syntax and is easily interoperable with C/C++.

## Features

### Variables

- declaring new variables: `int i = 0;`
- writing variables: `i = 5;`
- reading variables: `i = i + 5;`

### Functions

- declaring functions: `fun hello(float f) int { … }`
- calling functions: `hello(3.14);`

### Data Types

- Integer (32 bit): `int i = 0;`
- Float (32 bit): `float f = 0.0;`
- Booleans (1 bit): `bool b = true;`

### Link external object files

It is possible to link an object file with a program written in Neon,
so that one can call functions that are contained in the externally linked object file.

### Control flow

#### If statements

- if statements work just like in C
- if/else statements work just like in C

#### For loops

TODO

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
- use musl as the runtime library to create smaller binaries
- add reflection capabilities
    - inspect objects at runtime
    - get information about types
- make recursive function calls work

## Weird stuff

### Crash on recursive function optimization

This happens only on recursive functions, that call themselves.

```
define void @f() {
entry-f:
  call void @f()
  ret void
}
```
