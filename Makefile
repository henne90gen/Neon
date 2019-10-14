clean:
	rm build -rf

build:
	mkdir -p build; cd build; cmake -G"Ninja" -DRUN_CLANG_TIDY=OFF ..; ninja

test: build
	cd build; ninja test

run: build
	./build/src/main/Neon

clang-tidy: clean
	mkdir -p build; cd build; cmake -G"Ninja" -DRUN_CLANG_TIDY=ON ..; ninja

.PHONY: build
