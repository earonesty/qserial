SHELL:=/bin/bash
EMSDK_ENV ?= emsdk/emsdk_env.sh

.PHONY: all sdk test typecheck typescript-test

all: test

sdk: $(EMSDK_ENV)

emsdk/emsdk_env.sh:
	./get-sdk.sh

qserial.js: webasm.cpp ../qserial.hpp Makefile $(EMSDK_ENV)
	. $(EMSDK_ENV) && emcc -std=c++17 -s MODULARIZE=1 -s DISABLE_EXCEPTION_CATCHING=0 --bind -o qserial.js webasm.cpp

test: qserial.js
	node test.js

# Run npm ci before these targets.
typecheck:
	npm run typecheck

typescript-test: qserial.js
	npm test
