#!/bin/bash

find . -name '*.gcno' -exec gcov -s "$(realpath ..)" -r {} +

rm catch.hpp.gcov test_*.gcov

if [ -n "$CODECOV_TOKEN" ]; then
    codecov
fi
