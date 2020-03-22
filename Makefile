.PHONY: test coverage

build/CMakeCache.txt: CMakeLists.txt *.hpp
	cmake --version:
	mkdir -p build
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug

coverage: build/CMakeCache.txt
	cd build && make coverage

test: build/CMakeCache.txt
	cd build && cmake --build .
	cd build && ctest --verbose
