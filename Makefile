.PHONY: test
	
test:
	cmake -B build . -DCMAKE_BUILD_TYPE=Release
	cmake --build build
	cd build && ctest .
