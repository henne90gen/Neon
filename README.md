# Neon

Neon is an experimental programming language.
It has C-like syntax and is easily interoperable with C/C++.

## Features

### Variables

-   declaring new variables: `int i = 0;`
-   writing variables: `i = 5;`
-   reading variables: `i = i + 5;`
-   variable scoping
    -   variables that are defined inside of a scope are not accessible from outside that scope
    -   variables that are overriden in a scope are available again after that scope

### Functions

-   declaring functions: `fun hello(float f) int { … }`
-   calling functions: `hello(3.14);`

### Data Types

-   Integer (64 bit): `int i = 0;`
-   Float (64 bit): `float f = 0.0;`
-   Booleans (1 bit): `bool b = true;`
-   Arrays
    -   one dimensional arrays of all the basic data types are fully supported: `int[5] a;`

### Link external object files

It is possible to link an object file with a program written in Neon.
This makes it possible to call functions that are contained in the externally linked object file.

### Control flow

#### If statements

-   if statements work just like in C
-   if/else statements work just like in C
-   TODO if/else-if/…/else

#### For loops

-   for loops work just like in C
-   e.g. `for int i = 0; i < 5; i = i + 1 { … }`

### Imports

-   other Neon-files can be imported with the `import` keyword
-   the imported file is specified with a relative path to the current source file
-   e.g. `import "examples/functions.ne"`

## TODO / Ideas

-   standard library
    -   provide interface to important c functions
-   complex types
    -   arrays (multidimensional)
    -   compound types (structs)
-   think about interface definitions
    -   how to create them
    -   how to implement them
    -   how to match types that implement the same interface
    -   maybe a mixture between Go and Java?
-   add comments (# or //)
    -   single line
    -   multi line
-   importing files
    -   generate function in every module that contains all 'script' code
    -   generate function call to that function in place of the import statement
-   allow functions to be placed anywhere
    -   in global scope
    -   in function scope
    -   deeply nested in multiple functions, etc.
    -   make sure scoping of variables is done right.
    -   basically every function is a lambda
-   use musl as the runtime library to create smaller binaries
-   add reflection capabilities
    -   inspect objects at runtime
    -   get information about types
