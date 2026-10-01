[![Build Status](https://travis-ci.com/earonesty/qserial.svg?branch=master)](https://travis-ci.com/earonesty/qserial)
[![Code Coverage](https://codecov.io/gh/earonesty/qserial/branch/master/graph/badge.svg)](https://codecov.io/gh/earonesty/qserial)

# Simple C++ schema-driven serialization

 - Header-only library suitable for android/arm builds
 - Small, easy to understand
 - No pre-compilation of headers
 - No template specialization complexity

## Format overview

 - Field/value: similar to protobuf
 - 2-bit wire format selector (varint, varbyte, 32-bit, 64-bit)
 - varint field id (1st 32 fields addressable in 1 byte)
 - varint + length for str/binary
 - zigzag encoding for signed

## Interface

 - Construct instance of Schema class
 - call `encode()` to get encoder
    - call `set(enum, value)` to set fields
    - call `out()` to get output
 - call `decode(bytes)` to get decoder
    - call `get_xxx(enum)` to get fields
    - call `arr_get_xxx(enum, index)` to get repeated fields
    - call `arr_len(enum)` to get repeated field length

## Checks
 - Fields must be sequential and not repeat, unless marked repeated
 - Arrays/repeated fields must be sequential 
 - Unknown (forward compat) fields must be > schema max field
 - Wire type fixed via field type and length

## Speed
 - Zero-copy string/buffer access for byte decoding

The optional benchmark compares qserial with Google protobuf on the same field
values, including four 99-byte binary fields and a zigzag-encoded signed integer.
Each timed iteration constructs a message, serializes it, decodes it, and reads
one decoded integer. Both cases reuse their output buffer and validate all fields
outside the timed loop. Protobuf materializes its decoded message while qserial
uses its own decoder; this is an API-level round-trip comparison, not a claim of
identical decoding costs.

With CMake, a C++17 compiler, protobuf development libraries, and `protoc` installed:

```sh
cmake -S . -B build-bench -DCMAKE_BUILD_TYPE=Release -DBENCHMARK=ON
cmake --build build-bench
ctest --test-dir build-bench --output-on-failure
./build-bench/qserial-bench
```

On newer glibc systems, the bundled Catch version may fail to compile its POSIX
signal handler (`MINSIGSTKSZ` is no longer a constant). Add
`-DCMAKE_CXX_FLAGS=-DCATCH_CONFIG_NO_POSIX_SIGNALS` to the configure command to
disable that handler while retaining the tests and assertions.

Protobuf sources are generated from `bench/bench.proto` with the installed
compiler. Report the C++ compiler, optimization settings, protobuf version,
hardware, and repeated measurements alongside any performance comparison;
relative speed depends on the workload and environment.

## TODO:
 - stream i/o for encode/decode
