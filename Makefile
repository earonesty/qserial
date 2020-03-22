.PHONY: test
	
test:
	cmake --version
	mkdir -p build
	cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
	cmake --build build
	cd build && ctest --verbose
