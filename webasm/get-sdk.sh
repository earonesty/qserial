#!/bin/bash -e

git clone https://github.com/emscripten-core/emsdk.git
emsdk/emsdk install latest
emsdk/emsdk activate latest
