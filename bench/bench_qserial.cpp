#include <vector>
#include <iostream>
#include <chrono>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "qserial.hpp"
#include "bench.pb.h"

typedef std::vector<unsigned char> bytes;
using Schema = qserial::Schema;

const int LOOPS = 100000;

double timeit(int loops, std::function<void()> func) {
    using namespace std::chrono;
    using clock = high_resolution_clock;
    clock::time_point t1 = clock::now();
    for (int i=0; i < loops; ++i) {
        func();
    }
    clock::time_point t2 = clock::now();
    auto ret = duration_cast<duration<double>>(t2-t1);
    return ret.count()/loops;
}

TEST_CASE("serial::qs") {
    Schema s;
    s.add_field(0, Schema::Bin, false);
    s.add_field(1, Schema::Bin, false);
    s.add_field(2, Schema::UInt, true);
    s.add_field(3, Schema::SInt, true);
    s.add_field(4, Schema::Bin, false, true);
    bytes out;
    auto time = timeit(LOOPS, [s, &out]() {
        out.clear();
        auto enc = s.encode(out);

        enc.set(0, "hello");
        enc.set(1, "world world world world");
        enc.set(2, 23);
        enc.set(3, -23);
        enc.set(4, bytes(99));
        enc.set(4, bytes(99));
        enc.set(4, bytes(99));
        enc.set(4, bytes(99));

        auto ret = s.decode(out);
    });
    std::cout << "serial time: " << time << std::endl;
}

TEST_CASE("serial::pb") {
    Schema s;
    s.add_field(0, Schema::Bin, false);
    s.add_field(1, Schema::Bin, false);
    s.add_field(2, Schema::UInt, true);
    s.add_field(3, Schema::SInt, true);
    s.add_field(4, Schema::Bin, false, true);
    bytes out;
    auto time = timeit(LOOPS, [s, &out]() {
        out.clear();
        auto enc = s.encode(out);

        enc.set(0, "hello");
        enc.set(1, "world world world world");
        enc.set(2, 23);
        enc.set(3, -23);
        enc.set(4, bytes(99));
        enc.set(4, bytes(99));
        enc.set(4, bytes(99));
        enc.set(4, bytes(99));

        auto ret = s.decode(out);
    });
    std::cout << "serial time: " << time << std::endl;
}

