compile:
	clang++ -O0 compile.cpp -o compile && ./compile && llvm-objdump -d compile

llvm:
	clang++ -O0 -S llvm.cpp -emit-llvm && cat llvm.ll

link:
	clang++ -O0 link.cpp -o link ../main.ne.o && ./link

link-reverse:
	clang++ -O0 -c link-reverse.cpp -o link-reverse.o

time:
	./time.sh

struct_pointer:
	clang++ -O0 -S struct_pointer.cpp -emit-llvm && cat struct_pointer.ll

memory_test:
	clang++ -O0 memory_test.cpp -o memory_test && ./memory_test

custom_types:
	clang++ -O0 -S custom_types.cpp -emit-llvm

.PHONY: compile llvm link memory_test struct_pointer time
