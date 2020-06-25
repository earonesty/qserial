SHELL:=/bin/bash

all: test

sdk: emsdk/emsdk_env.sh

emsdk/emsdk_env.sh:
	./get-sdk.sh

qserial.js: webasm.cpp ../qserial.hpp Makefile emsdk/emsdk_env.sh
	. emsdk/emsdk_env.sh && emcc -std=c++17 -s MODULARIZE=1 -s DISABLE_EXCEPTION_CATCHING=0 --bind -o qserial.js webasm.cpp

test: qserial.js
	node test.js | tee >(cat >&2) | grep -q PASSED
