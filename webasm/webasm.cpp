#include <emscripten/bind.h>

#include "../qserial.hpp"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(qserial) {
    enum_<qserial::Schema::Type>("Schema.Type")
        ;
    class_<qserial::Schema::Serial>("Schema.Serial")
        ;
    class_<qserial::Schema>("Schema")
        .constructor<>()
        .function("add_field", &qserial::Schema::add_field)
        .function("encode", (qserial::Schema::Serial (qserial::Schema::*)() const) &qserial::Schema::encode)
        ;
}
