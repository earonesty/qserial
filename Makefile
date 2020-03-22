.PHONY: test
	
test:
	cmake --version
	mkdir -p build
	cd build && cmake .. -DCMAKE_BUILD_TYPE=Release
	cd build && cmake --build .
	cd build && ctest --verbose
