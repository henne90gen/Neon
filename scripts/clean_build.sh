#!/bin/bash

rm build -rf
mkdir -p build
cd build

#cmake -G"Ninja" -DCMAKE_BUILD_TYPE=Release -DRUN_CLANG_TIDY=ON ..
cmake -G"Ninja" -DCMAKE_BUILD_TYPE=Release -DRUN_CLANG_TIDY=OFF ..
time ninja -j 8 2> build_report.csv

ninja test
