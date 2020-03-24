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
 - ~2% slower than google protobuf

## TODO:
 - stream i/o for encode/decode
