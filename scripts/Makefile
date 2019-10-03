compile:
	clang++ -O0 compile.cpp -o compile && ./compile && llvm-objdump -d compile

llvm:
	clang++ -O0 -S llvm.cpp -emit-llvm && cat llvm.ll

link:
	clang++ -O0 link.cpp -o link ../main.ne.o && ./link

.PHONY: compile llvm link
