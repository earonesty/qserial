#include <vector>
#include <iostream>
#include <chrono>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <string>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "qserial.hpp"
#include "bench.pb.h"

typedef std::vector<unsigned char> bytes;
using Schema = qserial::Schema;

const int LOOPS = 100000;

double timeit(int loops, std::function<void()> func) {
    using namespace std::chrono;
    using clock = steady_clock;
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
    volatile uint64_t decoded_total = 0;
    auto time = timeit(LOOPS, [&s, &out, &decoded_total]() {
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
        decoded_total += ret.get_uint(2);
    });
    auto ret = s.decode(out);
    REQUIRE(ret.get_str(0) == "hello");
    REQUIRE(ret.get_str(1) == "world world world world");
    REQUIRE(ret.get_uint(2) == 23);
    REQUIRE(ret.get_sint(3) == -23);
    REQUIRE(ret.arr_len(4) == 4);
    for (size_t i = 0; i < 4; ++i) {
        REQUIRE(ret.get_bin(4, i) == bytes(99));
    }
    REQUIRE(decoded_total == 23 * LOOPS);
    std::cout << "qserial seconds/round-trip: " << time << std::endl;
}

TEST_CASE("serial::pb") {
    std::string out;
    volatile uint64_t decoded_total = 0;
    auto time = timeit(LOOPS, [&out, &decoded_total]() {
        out.clear();
        proto::benchy enc;
        enc.set_f0("hello");
        enc.set_f1("world world world world");
        enc.set_f2(23);
        enc.set_f3(-23);
        for (int i = 0; i < 4; ++i) {
            enc.add_f4(std::string(99, '\0'));
        }

        proto::benchy ret;
        if (!enc.SerializeToString(&out) || !ret.ParseFromString(out)) {
            throw std::runtime_error("protobuf round-trip failed");
        }
        decoded_total += ret.f2();
    });
    proto::benchy ret;
    REQUIRE(ret.ParseFromString(out));
    REQUIRE(ret.f0() == "hello");
    REQUIRE(ret.f1() == "world world world world");
    REQUIRE(ret.f2() == 23);
    REQUIRE(ret.f3() == -23);
    REQUIRE(ret.f4_size() == 4);
    for (int i = 0; i < 4; ++i) {
        REQUIRE(ret.f4(i) == std::string(99, '\0'));
    }
    REQUIRE(decoded_total == 23 * LOOPS);
    std::cout << "protobuf seconds/round-trip: " << time << std::endl;
}
