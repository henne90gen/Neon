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
	main.o


ld \
  -pie --eh-frame-hdr \
  -m elf_x86_64 \
  -dynamic-linker /lib64/ld-linux-x86-64.so.2 \
  -o a.out \
  /usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/../../../../lib64/Scrt1.o \
  /usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/../../../../lib64/crti.o \
  /usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/crtbeginS.o \
  -L/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0 \
  -L/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/../../../../lib64 \
  -L/usr/bin/../lib64 \
  -L/lib/../lib64 \
  -L/usr/lib/../lib64 \
  -L/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/../../.. \
  -L/usr/bin/../lib \
  -L/lib \
  -L/usr/lib \
  /tmp/math-8413f7.o \
  -lstdc++ -lm -lgcc_s -lgcc -lc -lgcc_s -lgcc \
  /usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/crtendS.o \
  /usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/../../../../lib64/crtn.o

ld \
  -pie --eh-frame-hdr \
  -m elf_x86_64 \
  -dynamic-linker /lib64/ld-linux-x86-64.so.2 \
  -o a.out \
  /usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/../../../../lib64/Scrt1.o \
  /usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/../../../../lib64/crti.o \
  /usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/crtbeginS.o \
  -L/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0 \
  -L/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/../../../../lib64 \
  -L/usr/bin/../lib64 -L/lib/../lib64 -L/usr/lib/../lib64 \
  -L/usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/../../.. \
  -L/usr/bin/../lib \
  -L/lib \
  -L/usr/lib \
  /tmp/math-b207ea.o \
  -lgcc --as-needed -lgcc_s --no-as-needed -lc -lgcc --as-needed -lgcc_s --no-as-needed \
  /usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/crtendS.o \
  /usr/bin/../lib64/gcc/x86_64-pc-linux-gnu/9.2.0/../../../../lib64/crtn.o
