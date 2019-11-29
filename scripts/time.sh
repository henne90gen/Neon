#!/bin/bash

# Grammar.cpp seems to take quite a long time to compile
# To verify this and to compare it to the compile time of Lexer.cpp (which seems quite normal)

echo
echo "clang++"
echo "Lexer"
(time clang++ ../src/main/Lexer.cpp -std=c++17 -shared -fPIC) 2>&1 | grep real
echo "Grammar"
(time clang++ ../src/main/Grammar.cpp -std=c++17 -shared -fPIC) 2>&1 | grep real

echo
echo "g++"
echo "Lexer"
(time g++ ../src/main/Lexer.cpp -std=c++17 -shared -fPIC) 2>&1 | grep real
echo "Grammar"
(time g++ ../src/main/Grammar.cpp -std=c++17 -shared -fPIC) 2>&1 | grep real
