#include <emscripten/bind.h>

#include <sstream>
#include <string>
#include <iomanip>

std::string buf2hex(const std::vector<uint8_t> &buf) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < buf.size(); ++i)
    {
        ss << std::setw(2) << static_cast<unsigned>(buf[i]);
    }
    return ss.str();
}

#include "../qserial.hpp"

#define THROW(m) {std::ostringstream s; s << m; throw std::runtime_error(s.str());}

using namespace emscripten;

static std::string getExceptionMessage(int exceptionPtr) {
    return std::string(reinterpret_cast<std::exception *>(exceptionPtr)->what());
}
template <typename T>
std::shared_ptr<std::vector<T>> convertJSArrayToNumberVector(const val & v) {
    const size_t l = v["length"].as<size_t>();

    auto rv = std::make_shared<std::vector<T>>();
    rv->resize(l);

    // Copy the array into our vector through the use of typed arrays.
    // The main issue however, is that if it contains something else than numbers (for example a string),
    // it will try to convert it as if using Number().
    // See https://www.ecma-international.org/ecma-262/6.0/#sec-%typedarray%.prototype.set-array-offset
    // and https://www.ecma-international.org/ecma-262/6.0/#sec-tonumber
    val memoryView{ typed_memory_view(l, rv->data()) };
    memoryView.call<void>("set", v);

    return rv;
}

EMSCRIPTEN_BINDINGS(qserial) {
    enum_<qserial::Schema::Type>("Type")
        .value("Bin", qserial::Schema::Type::Bin)
        .value("UInt", qserial::Schema::Type::UInt)
        .value("SInt", qserial::Schema::Type::SInt)
        .value("Dbl", qserial::Schema::Type::Dbl)
        .value("Flt", qserial::Schema::Type::Flt)
        ;
    class_<qserial::Schema::Serial>("Schema.Serial")
        .constructor<qserial::Schema>()
        .function("set",  optional_override([](qserial::Schema::Serial &self, size_t number, emscripten::val v) {
            auto typ = v.typeof().as<std::string>();
            if (typ == "string") {
                auto val = v.as<std::string>();
                self.set(number, val);
            } else if (typ == "number") {
                self.set(number, v.as<int64_t>());
            } else if (typ == "boolean") {
                self.set(number, v.as<bool>());
            }
         }))
        .function("out", optional_override([](qserial::Schema::Serial &self, bool check=true) {
            auto &br = self.out(check);
            return val(typed_memory_view(br.size(), br.data()));
         }))
        ;
    class_<qserial::Schema::Deserial>("Schema.Deserial")
        .function("get", optional_override([](qserial::Schema::Deserial &self, size_t number) {
                if (number >= self.schema.fields.size()) {
                    throw std::runtime_error("field out of range");
                }
                auto typ = self.schema.fields[number].type;
                if (typ == qserial::Schema::Type::Bin) {
                    auto ret = self.get_str(number);
                    printf("HERE %s\n", ret.c_str());
                    return emscripten::val(self.get_str(number));
                } else if (typ == qserial::Schema::Type::UInt) {
                    return emscripten::val(self.get_uint(number));
                } else if (typ == qserial::Schema::Type::SInt) {
                    return emscripten::val(self.get_sint(number));
                }
                THROW("unknown type: " << typ);
         }))
        ;
    class_<qserial::Schema>("Schema")
        .constructor<>()
        .function("get_type", optional_override([](qserial::Schema &self, size_t number) {
                if (number >= self.fields.size()) {
                    throw std::runtime_error("field out of range");
                }
                return int(self.fields[number].type);
         }))
        .function("add_field", &qserial::Schema::add_field)
        .function("encode", (qserial::Schema::Serial (qserial::Schema::*)() const) &qserial::Schema::encode)
        .function("decode", optional_override([](qserial::Schema &self, emscripten::val dat, bool check=true) {
            auto vec = convertJSArrayToNumberVector<uint8_t>(dat);
            auto ref_vec = std::shared_ptr<qserial::bytes>(vec);
            return self.decode(ref_vec, check);
         }))
        ;
    function("exceptionMsg", &getExceptionMessage);
};
