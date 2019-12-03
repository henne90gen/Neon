clean:
	rm build -rf

cmake:
	mkdir -p build; cd build; cmake -G"Ninja" -DRUN_CLANG_TIDY=OFF -DCMAKE_BUILD_TYPE=Debug ..;

build:
	cd build; ninja Neon

build-test:
	cd build; ninja Tests

test: build-test
	cd build; ninja test

run: build
	./build/src/main/Neon

generate-parser:
	cd src/main && python ../../scripts/parser_generator.py

clang-tidy: clean
	mkdir -p build; cd build; cmake -G"Ninja" -DRUN_CLANG_TIDY=ON ..; ninja

docs:
	doxygen

.PHONY: build docs
