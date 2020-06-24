sdk: emsdk/emsdk_env.sh

emsdk/emsdk_env.sh:
	./get-sdk.sh

qserial.js: webasm.cpp ../qserial.hpp Makefile emsdk/emsdk_env.sh
	. emsdk/emsdk_env.sh && emcc -std=c++17 -s MODULARIZE=1 --bind -o qserial.js webasm.cpp

test: qserial.js
	node test.js
