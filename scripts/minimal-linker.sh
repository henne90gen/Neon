#!/bin/bash

# Minimal linker command to create an executable from an object file
# The object file has to contain a main-function.

ld \
	-dynamic-linker /lib64/ld-linux-x86-64.so.2 \
	/lib64/crt1.o \
	/lib64/crti.o \
	/lib64/crtn.o \
	-L/usr/lib/gcc/x86_64-pc-linux-gnu/9.1.0/ \
	-lc -lgcc -lgcc_s \
  -o main \
	main.ne.o

