#!/bin/bash

rm build -rf
mkdir -p build
cd build

cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DRUN_CLANG_TIDY=ON ..
#cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DRUN_CLANG_TIDY=OFF ..
time make -j 8 2> build_report.csv

make test
