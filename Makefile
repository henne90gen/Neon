all: clean cmake build test-all

clean:
	rm build -rf

cmake:
	mkdir -p build; cd build; cmake -G"Ninja" -DRUN_CLANG_TIDY=OFF -DCMAKE_BUILD_TYPE=Debug ..;

build:
	cd build; ninja Neon

build-test:
	cd build; ninja Tests LexerFuzzTests ParserFuzzTests NeonTester

test-all: build-test
	cd build; ninja test

test-unit: build-test
	cd build/src/test/unit; ./Tests

test-fuzz-lexer: build-test
	./build/src/test/fuzz/LexerFuzzTests src/test/fuzz/lexer-corpus tests -dict=src/test/fuzz/lexer_fuzz_dictionary -print_final_stats=1 #-workers=8 -jobs=8

test-fuzz-parser: build-test
	./build/src/test/fuzz/ParserFuzzTests src/test/fuzz/parser-corpus tests -dict=src/test/fuzz/parser_fuzz_dictionary -print_final_stats=1 #-workers=8 -jobs=8

test-integration: build-test
	./build/src/test/integration/NeonTester

run: build
	./build/src/main/Neon && echo "" && ./neon-build/main; echo $$?

generate-parser:
	cd src/main && python ../../scripts/parser_generator.py

clang-tidy: clean
	mkdir -p build; cd build; cmake -G"Ninja" -DRUN_CLANG_TIDY=ON ..; ninja > clang-tidy-report.txt

docs:
	doxygen

.PHONY: build docs
