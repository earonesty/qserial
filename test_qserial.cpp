#include <vector>
#include <iostream>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "qserial.hpp"

typedef std::vector<unsigned char> bytes;
using Schema = qserial::Schema;

std::string buf2hex(const unsigned char *buf, size_t len) {
    if (!len)
        return("");
    std::string ret; ret.resize(len*2);
    #define _i2h(c) ((c) > 9 ? 'A'+(c)-10 : '0' + (c))
    for (size_t i=0;i<len;++i) {
        ret[i*2] = _i2h((buf[i] & 0xF0) >> 4);
        ret[i*2+1] = _i2h(buf[i] & 0x0F);
    }
    return ret;
}
std::string buf2hex(const bytes &buf) {
    return buf2hex(buf.data(), buf.size());
}

TEST_CASE("serial::vint") {
    bytes out;
    Schema::encode_vint(out, 0);
    Schema::encode_vint(out, 9999);
    CHECK(out.size() == 3);
    size_t i = 0;
    uint64_t ret;
    ret = Schema::decode_vint(out, &i, out.size());
    CHECK(ret == 0);
    ret = Schema::decode_vint(out, &i, out.size());
    CHECK(ret == 9999);
}

TEST_CASE("serial::fint") {
    bytes out;
    Schema::encode_fixed(out, (uint32_t) 44);
    Schema::encode_fixed(out, (uint64_t) 99);
    CHECK(out.size() == 12);
    uint32_t ret0;
    uint64_t ret1;
    size_t off = 0;
    Schema::decode_fixed(out, &off, &ret0);
    CHECK(ret0 == 44);
    Schema::decode_fixed(out, &off, &ret1);
    CHECK(ret1 == 99);
}
TEST_CASE("serial::uint_max") {
    Schema s;
    s.add_field(9, Schema::UInt, true);

    auto enc = s.encode();

    enc.set(9, UINT64_MAX);

    std::cout << "uint-max: " << buf2hex(enc.out()) << std::endl;

    auto ret = s.decode(enc.out());

    CHECK(UINT64_MAX == ret.get_uint(9));
}

TEST_CASE("serial::zero") {
    Schema s;
    s.add_field(9, Schema::UInt, true);

    auto enc = s.encode();

    enc.set(9, 0);

    std::cout << "zero: " << buf2hex(enc.out()) << std::endl;

    auto ret = s.decode(enc.out());

    CHECK(0 == ret.get_uint(9));
    CHECK("2400" == buf2hex(enc.out()));
}


TEST_CASE("serial::uint") {
    Schema s;
    s.add_field(0, Schema::UInt, false);
    s.add_field(9, Schema::UInt, true);
    auto enc = s.encode();

    enc.set(0, 33);
    enc.set(9, (uint64_t) 23894723894787348);

    auto ret = s.decode(enc.out());

    CHECK(33 == ret.get_uint(0));
    CHECK(23894723894787348UL == ret.get_uint(9));
    CHECK(33 == ret.get_sint(0));
    CHECK(23894723894787348UL == ret.get_sint(9));
}

TEST_CASE("serial::sint") {
    Schema s;
    s.add_field(0, Schema::SInt, false);
    s.add_field(9, Schema::SInt, true);
    auto enc = s.encode();

    enc.set(0, -23);
    std::cout << "int -32: " << buf2hex(enc.out(false)) << std::endl;
    enc.set(9, (uint64_t) 23897137894);

    auto ret = s.decode(enc.out());

    CHECK(-23 == ret.get_sint(0));
    CHECK(23897137894 == ret.get_sint(9));
    CHECK(23897137894 == ret.get_uint(9));
}

TEST_CASE("serial::str") {
    Schema s;
    s.add_field(0, Schema::Bin, false);
    s.add_field(1, Schema::Bin, false);
    bytes out;
    auto enc = s.encode(out);

    enc.set(0, "hello");
    enc.set(1, "world world world world");

    auto ret = s.decode(out);

    CHECK("hello" == ret.get_str(0));
    CHECK("world world world world" == ret.get_str(1));
}

TEST_CASE("serial::empty") {
    Schema s;
    s.add_field(0, Schema::Bin, false);
    auto enc = s.encode();
    enc.set(0, "");
    std::cout << "empty string : " << buf2hex(enc.out()) << std::endl;
    auto ret = s.decode(enc.out());
    CHECK("" == ret.get_str(0));
    CHECK("0100" == buf2hex(enc.out()));
}

TEST_CASE("serial::fix") {
    Schema s;
    s.add_field(0, Schema::Bin, false);
    s.add_field(1, Schema::Bin, false);
    s.add_field(2, Schema::UInt, true);
    s.add_field(3, Schema::SInt, true);
    s.add_field(4, Schema::Bin, false);
    bytes out;
    auto enc = s.encode(out);

    enc.set(0, "hello");
    enc.set(1, "world world world world");
    enc.set(2, 23);
    enc.set(3, -23);
    enc.set(4, bytes(99));

    auto ret = s.decode(out);

    CHECK("hello" == ret.get_str(0));
    CHECK("world world world world" == ret.get_str(1));
    CHECK(23 == ret.get_sint(2));
    CHECK(-23 == ret.get_sint(3));
    const char *buf;
    size_t len;
    ret.get(0, &buf, &len);
    CHECK(std::string(buf, buf + len) == "hello");
    CHECK(99 == ret.get_bin(4).size());
}

TEST_CASE("serial::optional") {
    Schema s;
    s.add_field(0, Schema::Bin, false);
    s.add_field(1, Schema::SInt, false);
    auto enc = s.encode();
    auto nada = enc.out();
    auto dec = s.decode(nada);
    CHECK(dec.get_str(0) == "");
    CHECK(dec.get_bin(0) == bytes());
    CHECK(dec.get_sint(1,-1) == -1);
    CHECK(dec.get_uint(1,0) == 0);
}


TEST_CASE("serial::bad") {
    Schema s;
    s.add_field(0, Schema::Bin, true);
    auto enc = s.encode();
    auto ret0 = s.decode(bytes(), false);
    CHECK_THROWS(ret0.get_bin(0));

    CHECK_THROWS(enc.out());
    enc.set(0, "");
    auto ret = s.decode(enc.out());
    CHECK("" == ret.get_str(0));
    CHECK_THROWS(ret.get_bin(999999999));
    CHECK_THROWS(s.decode(bytes()));
    CHECK_THROWS(s.decode(bytes({4,3,2,1})));
    CHECK_THROWS(s.decode(bytes({1,2,1})));
    CHECK_THROWS(s.decode(bytes({0,2,1})));
    CHECK_THROWS(s.decode(bytes({255,255,255,255})));

    s.add_field(9, Schema::Bin, true);
    auto enc2 = s.encode();
    enc2.set(0, "");
    CHECK_THROWS(enc2.set(9, 32));
    CHECK_THROWS(enc2.set(10, ""));
    CHECK_THROWS(enc2.out());
    CHECK_THROWS(s.decode(enc2.out(false)));

    auto ret2 = s.decode(enc2.out(false), false);
    CHECK("" == ret2.get_str(0));
}


TEST_CASE("serial::flt/dbl") {
    Schema s;
    s.add_field(9, Schema::Flt, true);
    s.add_field(10, Schema::Dbl, true);

    auto enc = s.encode();

    enc.set(9, FLT_MAX);
    enc.set(10, DBL_MAX);

    std::cout << "flt/dbl-max: " << buf2hex(enc.out()) << std::endl;

    auto ret = s.decode(enc.out());

    CHECK(FLT_MAX == ret.get_flt(9));
    CHECK(DBL_MAX == ret.get_dbl(10));
}

TEST_CASE("serial::arr") {
    Schema s;
    s.add_field(9, Schema::Flt, true, true);
    s.add_field(10, Schema::Dbl, false, true);
    s.add_field(11, Schema::Bin, false, true);

    auto enc = s.encode();

    enc.set(9, FLT_MAX);
    enc.set(9, FLT_MAX);
    enc.set(9, FLT_MAX);
    enc.set(10, DBL_MAX);
    enc.set(10, DBL_MAX);
    enc.set(10, DBL_MAX);
    enc.set(11, "hi");
    enc.set(11, "hi");
    enc.set(11, "hi");

    std::cout << "array: " << buf2hex(enc.out()) << std::endl;

    auto ret = s.decode(enc.out());
    CHECK(3 == ret.arr_len(9));
    CHECK(3 == ret.arr_len(10));
    CHECK(3 == ret.arr_len(11));
    CHECK(FLT_MAX == ret.arr_get_flt(9, 0));
    CHECK(FLT_MAX == ret.arr_get_flt(9, 1));
    CHECK(FLT_MAX == ret.arr_get_flt(9, 2));
    CHECK(DBL_MAX == ret.arr_get_dbl(10, 0));
    CHECK(DBL_MAX == ret.arr_get_dbl(10, 1));
    CHECK(DBL_MAX == ret.arr_get_dbl(10, 2));
    CHECK("hi" == ret.get_str(11, 0));
    CHECK("hi" == ret.get_str(11, 1));
    CHECK("hi" == ret.get_str(11, 2));
}
