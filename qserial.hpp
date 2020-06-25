#include <vector>
#include <memory>
#include <string>
#include <algorithm>

namespace qserial {
/*
 * Simple but limited schema-driven serialization, header-only < 500 LOC.
 *
 * Features:
 *
 *  - Extensible typing system
 *  - Easy-to-understand length-prefixed encoding
 *  
 * Limitations:
 *
 *  - Use your own enum for field names
 *  - Max INT64_MAX fields
 *  - Max INT64_MAX per field
 *  
 */

using bytes = std::vector<uint8_t>;

class Error : public std::runtime_error {
    public:
        explicit Error(const char *msg) : std::runtime_error(msg) {}
};


class Schema {
  public:
    enum Type {
        None = 0,           // none
        UInt = 1,           // unsigned int
        SInt = 2,           // signed int
        Bin = 3,            // bytes or string
        Dbl = 4,            // double-precision IEEE754
        Flt = 5,            // float-precision IEEE754
    };

    // variable width integer decode/encode
    static uint64_t decode_vint(const bytes &dat, size_t *ptr, size_t end) {
         int i = 0;
         bool ext = false;
         uint64_t ret = 0;
         do {
            if (*ptr >= end) {
                throw Error("invalid vint");
            }
            uint8_t b = dat[*ptr];
            ext = b & 128;
            b &= ~128;
            ret += ((uint64_t)b) << 7*i;
            ++*ptr;
            ++i;
        } while(ext);
        return ret;
    }

    static void encode_vint(bytes &out, uint64_t num) {
        do {
            auto b = num & 127;
            num >>= 7;
            if (num) {
                b |= 128;
            }
            out.push_back(b);
        } while(num);
    }  

    // fixed width integer decode/encode
    template <typename T>
    static void decode_fixed(const bytes &dat, size_t *off, T *out) {
        *out = 0;
        for (size_t i=0; i < sizeof(T); ++i) {
            uint8_t b = dat[*off+i];
            *out += ((uint64_t)b) << 8*i;
        }
        *off += sizeof(T);
    }

    template <typename T>
    static void encode_fixed(bytes &out, T num) {
        for (size_t i=0; i < sizeof(T); ++i) {
            uint8_t b = num & 255;
            num >>= 8;
            out.push_back(b);
        }
    }  

    template<typename T1, typename T2>
    static T2 bitcast(T1 f) {
        union {T1 f; T2 i;} r;
        r.f = f;
        return r.i;
    }


  private:  
    enum Encoding {
        EVInt = 0,         // variable-width integer
        EVar = 1,          // vint length-prefixed
        E4 = 2,            // 4-byte fixed
        E8 = 3,            // 8-byte fixed
    };

    // schema specification entry
    class Entry {
      public:
        Type type = None;
        bool required = false;
        bool repeated = false;
    };
 
    // typed union for parsed values
    class Val {
      public:
        Type type;
        union {
            struct {
                const uint8_t *dat;
                size_t len;
            };
            int64_t ival;
            uint64_t uval;
            float fval;
            double dval;
        };
        Val() : type(None) {};
        Val(Type type, const uint8_t *dat, size_t len) : type(type), dat(dat), len(len) {}
        Val(Type type, int64_t dat) : type(type), ival(dat) {};
        Val(Type type, uint64_t dat) : type(type), uval(dat) {};
        Val(Type type, float dat) : type(type), fval(dat) {};
        Val(Type type, double dat) : type(type), dval(dat) {};
    };

  public:
    // container for parsed values
    class Deserial {
        friend class Schema;
      public:
        const Schema &schema;

      private:
        const std::shared_ptr<bytes> ptr;
        const bytes &in;
        std::vector<std::vector<Val>> vals;
        int64_t _prev = -1;

        int64_t decode_int64(uint64_t dec) {
            // see "zigzag" encoding
            dec = (dec >> 1) ^ -(dec & 1);
            return (int64_t) dec;
        }

        void decode(size_t num, const Entry &ent, Encoding enc, size_t *offset) {
            if (num >= INT64_MAX) {
                throw Error("field num too high");
            }
            if ((int)enc >= 4) {
                throw Error("invalid encoding");
            }
            uint64_t vint;
            uint32_t v32;

            if (enc == E4) {
                if (*offset + 4 > in.size()) 
                    throw Error("invalid field length 4");
                decode_fixed(in, offset, &v32);
            } else if (enc == E8) {
                if (*offset + 8 > in.size())
                    throw Error("invalid field length 8");
                decode_fixed(in, offset, &vint);
            } else {
                vint = decode_vint(in, offset, in.size());
            }

            if (((int64_t)num) <= _prev) {
                throw Error("decode sequence error");
            }

            if (num>=vals.size()) {
                vals.resize(num+1);
            }
            if (enc == EVar) {
                size_t len = vint;
                if (*offset + len > in.size()) {
                    throw Error("invalid field length");
                }
                if (ent.type == Bin) {
                    vals[num].emplace_back(ent.type, in.data() + *offset, len);
                } else {
                    vals[num].emplace_back(ent.type, in.data() + *offset, len);
                    #ifdef QSERIAL_STRICT
                    // setting this breaks forward compatibility
                    throw Error("invalid entry type/encoding pair");
                    #endif
                }
                *offset += len;
            } else if (ent.type == SInt and enc == EVInt) {
                vals[num].emplace_back(ent.type, decode_int64(vint));
            } else if (ent.type == UInt and enc == EVInt) {
                vals[num].emplace_back(ent.type, vint);
            } else if (ent.type == Flt and enc == E4) {
                vals[num].emplace_back(ent.type, bitcast<uint32_t, float>(v32));
            } else if (ent.type == Dbl and enc == E8) {
                vals[num].emplace_back(ent.type, bitcast<uint64_t, double>(vint));
            } else {
                #ifdef QSERIAL_STRICT
                // setting this breaks forward compatibility
                throw Error("invalid entry type/encoding pair");
                #endif
            }
        }

        bool check_range(size_t number, size_t index) const {
            if (number >= schema.size()) {
                throw Error("field out of range");
            }
            if (number >= vals.size()) {
                if (schema.fields[number].required) {
                    throw Error("missing required field");
                }
                return false;
            }
            if (index >= vals[number].size()) {
                throw Error("invalid array index");
            }
            return number < vals.size();
        }

        explicit Deserial(const Schema &schema, const bytes &data) : schema(schema), in(data) {}
        explicit Deserial(const Schema &schema, std::shared_ptr<bytes> data) : schema(schema), ptr(data), in(*data) {}

      public:
        void get(size_t number, size_t index, const uint8_t **buf, size_t *len) const {
            if (!check_range(number, index)) {
                *buf = nullptr;
                *len = 0;
                return;
            }
            if (vals[number][index].type != Bin) {
                throw Error("expected string");
            }
            *buf = vals[number][index].dat;
            *len = vals[number][index].len;
        }
        void get(size_t number, size_t index, const char **buf, size_t *len) const {
            get(number, index, (const uint8_t **) buf, len);
        }
        template<typename T>
        void get(size_t number, T **buf, size_t *len) const {
            get(number, 0, buf, len);
        }

        template <typename T>
        bool get(size_t number, size_t index, T *ret) const {
            if (!check_range(number, index)) {
                return false;
            }
            if (vals[number][0].type == SInt) {
                *ret = vals[number][0].ival;
            } else if (vals[number][0].type == UInt) {
                *ret = vals[number][0].uval;
            } else if (vals[number][0].type == Flt) {
                *ret = vals[number][0].fval;
            } else if (vals[number][0].type == Dbl) {
                *ret = vals[number][0].dval;
            } else {
                throw Error("invalid type");
            }
            return true;
        }

        template <typename T>
        bool get(size_t number, T *ret) const {
            return get(number, 0, ret);
        }

        std::string get_str(size_t number, size_t index=0) const {
            if (!check_range(number, index)) 
                return std::string();
            if (vals[number][index].type != Bin) 
                throw Error("expected string");
            return std::string(vals[number][index].dat, vals[number][index].dat + vals[number][index].len);
        }

        bytes get_bin(size_t number, size_t index=0) const {
            if (!check_range(number, index)) 
                return bytes();
            if (vals[number][index].type != Bin) 
                throw Error("expected string");
            return bytes(vals[number][index].dat, vals[number][index].dat + vals[number][index].len);
        }

#define GET_NUM(prefix, type)\
        type get_##prefix(size_t number, type def=0) const { \
            type ret; \
            if (!get(number, &ret)) \
                return def; \
            return ret; \
        } \
        type arr_get_##prefix(size_t number, size_t index, type def=0) const { \
            type ret; \
            if (!get(number, index, &ret)) \
                return def; \
            return ret; \
        } \

        GET_NUM(sint, int64_t)
        GET_NUM(uint, uint64_t)
        GET_NUM(flt, float)
        GET_NUM(dbl, double)

        size_t arr_len(size_t number) const {
            check_range(number, 0);
            return vals[number].size();
        }

        bool is_valid(const Schema & schema) const {
            for (size_t i = 0; i < schema.fields.size(); ++i) {
                if (schema.fields[i].required && ( i>= vals.size() || vals[i].size() == 0 ) ) {
                    return false;
                }
            }
            return true;
        }
    };

    void encode_type(size_t number, bytes &out, Type type, size_t size) const {
        if (number >= fields.size()) {
            throw Error("field out of range");
        }
        if (fields[number].type != type) {
            // printf("type: %i != %i\n", fields[number].type, type);
            throw Error("invalid type for field");
        }
        uint64_t dat = number;
        dat <<= 2;
        uint8_t enc;
        if (type == SInt || type == UInt) {
            enc = EVInt;
        } else if (type == Bin) {
            enc = EVar;
        } else if (type == Flt) {
            enc = E4;
        } else if (type == Dbl) {
            enc = E8;
        } else {
            throw Error("unknown field type");
        }
        dat |= enc;
        encode_vint(out, dat);
        if (enc == EVar) {
            encode_vint(out, size);
        }
    }

  public:
    // serialization builder class
    class Serial {
      public:
        const Schema &schema;
      private:

        // caller can use this without pre-specified memory or with
        std::shared_ptr<bytes> _mem;
        bytes &_out;
        std::vector<bool> _fdset;
        int64_t _prev = -1;

        const Entry &field(size_t num) {
            if (num >= schema.fields.size()) {
                throw Error("field num too high for schema");
            }
            return schema.fields[num];
        }

        void check_seq(size_t number) {
            if (((int64_t)number) <= _prev) {
                if (field(number).repeated) {
                    return;
                }
                throw Error("fields must be set sequentially");
            }
            _prev = number;
        }

        void set_type(size_t number, Type type, size_t size) {
            check_seq(number);
            schema.encode_type(number, _out, type, size);
            _fdset[number]=true;
        }
 
      public:
        
        Serial(const Schema& schema, bytes &out) : schema(schema), _out(out), _fdset(schema.fields.size()) {
        }
        explicit Serial(const Schema& schema) : schema(schema), _mem(new bytes), _out(*_mem), _fdset(schema.fields.size()) {
        }
       

        void set(size_t number, const uint8_t *value, size_t len) {
            set_type(number, Bin, len);
            auto offset = _out.size();
            _out.resize(offset + len);
            std::copy(value, value + len, _out.begin() + offset);
        }


        void set(size_t number, const std::string &value) {
            set(number, (const uint8_t *) value.data(), value.size());
        }

        void set(size_t number, const bytes &value) {
            set(number, value.data(), value.size());
        }

        void set(size_t number, int64_t value) {
            if (field(number).type == UInt) {
                set(number, (uint64_t) value);
                return;
            }
            uint64_t xval = value;
            xval = -(xval >> 63) ^ (xval << 1);
            set_type(number, SInt, 0);
            schema.encode_vint(_out, xval);
        }

        void set(size_t number, uint64_t value) {
            if (field(number).type == SInt) {
                set(number, (int64_t) value);
                return;
            }
            set_type(number, UInt, 0);
            schema.encode_vint(_out, value);
        }

        void set(size_t number, int value) {
            set(number, (int64_t) value);
        }

        void set(size_t number, double value) {
            if (field(number).type == Flt) {
                set(number, (double) value);
                return;
            }
            uint64_t valx = bitcast<double, uint64_t>(value);
            set_type(number, Dbl, 0);
            schema.encode_fixed(_out, valx);
        }

        void set(size_t number, float value) {
            if (field(number).type == Dbl) {
                set(number, (float) value);
                return;
            }
            uint32_t valx = bitcast<float, uint32_t>(value);
            set_type(number, Flt, 0);
            schema.encode_fixed(_out, valx);
        }

   
        bytes &out(bool check=true) {
            // output bytes: if check is true, will raise on missing fields
            if (check && !is_valid() ) {
                throw Error("missing required fields");
            }
            return _out;
        }

        bool is_valid() {
            for (size_t i = 0; i < schema.fields.size(); ++i) {
                if (schema.fields[i].required && (i>=_fdset.size() || !_fdset[i])) {
                    return false;
                }
            }
            return true;
        }
    };

 public:
    std::vector<Entry> fields;
    size_t size() const {return fields.size();};
    Schema() {}

     // schema specification entry
    struct Field {
        size_t number;
        Type type;
        bool required;
        bool repeated;
    };

    // cppcheck-suppress noExplicitConstructor
    Schema(const std::initializer_list<Field> &in) {        // NOLINT
        for (auto e: in) {
            add_field(e.number, e.type, e.required, e.repeated);
        }
    }

    static void decode_type(const bytes &dat, Encoding *encoding, size_t *field_number, size_t *ptr) {
        uint64_t num = decode_vint(dat, ptr, dat.size());
        *encoding = static_cast<Encoding>(num & 3);
        num >>= 2;
        *field_number = num;
    }

    Serial encode(bytes &out) const {
        return Serial(*this, out);
    }
    Serial encode() const {
        return Serial(*this);
    }

    Deserial decode(std::shared_ptr<bytes> value, bool check=true) const {
        Deserial ret(*this, value);
        decode(ret, check);
        return ret;
    }

    Deserial decode(const bytes &value, bool check=true) const {
        Deserial ret(*this, value);
        decode(ret, check);
        return ret;
    }

    void decode(Deserial &ret, bool check) const {
        size_t i = 0;
        while (i < ret.in.size()) {
            Encoding enc;
            size_t num;
            decode_type(ret.in, &enc, &num, &i);
            if (num >= size()) {
                throw Error("field number too large");
            }
            ret.decode(num, fields[num], enc, &i);
        }
        if (check && !ret.is_valid(*this)) {
            throw Error("missing required fields");
        }
    }

    void add_field(size_t number, Type type, bool required, bool repeated=false) {
        if (number >= fields.size()) {
            fields.resize(number + 1);
        }
        fields[number] = {type, required, repeated};
    }

};

};
