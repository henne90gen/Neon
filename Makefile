all: clean cmake build test-all

clean:
	rm build -rf

cmake:
	mkdir -p build; cd build; cmake -G"Ninja" -DRUN_CLANG_TIDY=OFF -DCMAKE_BUILD_TYPE=Debug ..;

build:
	cd build; ninja Neon

build-test:
	cd build; ninja Tests FuzzTests NeonTester

test-all: build-test test-unit fuzz-test integration-test

test-unit:
	cd build/src/test; ./Tests

fuzz-test:
	cd build/src/test; ./FuzzTests

integration-test:
	./build/src/tester/NeonTester

run: build
	./build/src/main/Neon && echo "" && ./neon-build/main; echo $$?

generate-parser:
	cd src/main && python ../../scripts/parser_generator.py

clang-tidy: clean
	mkdir -p build; cd build; cmake -G"Ninja" -DRUN_CLANG_TIDY=ON ..; ninja > clang-tidy-report.txt

docs:
	doxygen

.PHONY: build docs
