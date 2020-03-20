#include <vector>
#include <memory>
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
 *  - Max UINT64_MAX fields
 *  - Max UINT64_MAX per field
 *  - No floating-point yet
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
    
  private:  
    enum Encoding {
        EVInt = 0,         // variable-width integer
        EVar = 1,          // vint length-prefixed
    };

    // schema specification entry
    class Entry {
      public:
        Type type = None;
        bool required = false;
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
        };
        Val() : type(None) {};
        Val(Type type, const uint8_t *dat, size_t len) : type(type), dat(dat), len(len) {}
        Val(Type type, int64_t dat) : type(type), ival(dat) {};
        Val(Type type, uint64_t dat) : type(type), uval(dat) {};
    };

    // container for parsed values
    class Deserial {
        friend class Schema;

        const Schema &schema;
        const bytes &in;
        std::vector<Val> vals;
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
            if (!(enc == EVar || enc == EVInt)) {
                throw Error("invalid encoding");
            }
            uint64_t vint = decode_vint(in, offset, in.size());

            if (((int64_t)num) <= _prev) {
                throw Error("decode sequence error");
            }

            if (num>=vals.size()) {
                vals.resize(num+1);
            }
            if (ent.type == Bin) {
                size_t len = vint;
                if (*offset + len > in.size()) {
                    throw Error("invalid field length");
                }
                vals[num] = Val(ent.type, in.data() + *offset, len);
                *offset += len;
            } else if (ent.type == SInt) {
                vals[num] = Val(ent.type, decode_int64(vint));
            } else if (ent.type == UInt) {
                vals[num] = Val(ent.type, vint);
            } else {
                throw Error("invalid entry type");
            }
        }

        bool check_range(size_t number) {
            if (number >= schema.size()) {
                throw Error("field out of range");
            }
            if (number >= vals.size() && schema.fields[number].required) {
                throw Error("missing required field");
            }
            return number < vals.size();
        }

        explicit Deserial(const Schema &schema, const bytes &data) : schema(schema), in(data) {}

      public:
        void get(size_t number, const uint8_t **buf, size_t *len) {
            if (!check_range(number)) {
                *buf = nullptr;
                *len = 0;
                return;
            }
            if (vals[number].type != Bin) {
                throw Error("expected string");
            }
            *buf = vals[number].dat;
            *len = vals[number].len;
        }
        void get(size_t number, const char **buf, size_t *len) {
            get(number, (const uint8_t **) buf, len);
        }
 
        std::string get_str(size_t number) {
            if (!check_range(number)) {
                return std::string();
            }
            if (vals[number].type != Bin) {
                throw Error("expected string");
            }
            return std::string(vals[number].dat, vals[number].dat + vals[number].len);
        }

        bytes get_bin(size_t number) {
            if (!check_range(number)) {
                return bytes();
            }
            if (vals[number].type != Bin) {
                throw Error("expected string");
            }
            return bytes(vals[number].dat, vals[number].dat + vals[number].len);
        }
 
        int64_t get_sint(size_t number, int64_t def=0u) {
            if (!check_range(number)) {
                return def;
            }
            if (vals[number].type == SInt) {
                return vals[number].ival;
            } else if (vals[number].type == UInt) {
                return vals[number].uval;
            } else {
                throw Error("invalid type");
            }
        }

        uint64_t get_uint(size_t number, int64_t def=0) {
            if (!check_range(number)) {
                return def;
            }
            if (vals[number].type == SInt) {
                return vals[number].ival;
            } else if (vals[number].type == UInt) {
                return vals[number].uval;
            } else {
                throw Error("invalid type");
            }
        }

        bool is_valid(const Schema & schema) {
            for (size_t i = 0; i < schema.fields.size(); ++i) {
                if (schema.fields[i].required && ( i>= vals.size() || vals[i].type == None ) ) {
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
            throw Error("invalid type for field");
        }
        uint64_t dat = number;
        dat <<= 2;
        uint8_t enc;
        if (type == SInt || type == UInt) {
            enc = EVInt;
        } else if (type == Bin) {
            enc = EVar;
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
        const Schema &schema;

        // caller can use this without pre-specified memory or with
        std::shared_ptr<bytes> _mem;
        bytes &_out;
        std::vector<bool> _fdset;
        int64_t _prev = -1;

        void check_seq(size_t number) {
            if (number >= INT64_MAX) {
                throw Error("field num too high");
            }
            if (((int64_t)number) <= _prev) {
                throw Error("fields must be set sequentially");
            }
            _prev = number;
        }

      public:
        
        Serial(const Schema& schema, bytes &out) : schema(schema), _out(out), _fdset(schema.fields.size()) {
        }
        explicit Serial(const Schema& schema) : schema(schema), _mem(new bytes), _out(*_mem), _fdset(schema.fields.size()) {
        }
       

        void set(size_t number, const std::string &value) {
            check_seq(number);
            schema.encode_type(number, _out, Bin, value.size());
            auto offset = _out.size();
            _out.resize(offset + value.size());
            std::copy(value.begin(), value.end(), _out.begin() + offset);
            _fdset[number]=true;
        }

        void set(size_t number, const bytes &value) {
            check_seq(number);
            schema.encode_type(number, _out, Bin, value.size());
            auto offset = _out.size();
            _out.resize(offset + value.size());
            std::copy(value.begin(), value.end(), _out.begin() + offset);
            _fdset[number]=true;
        }

        void set(size_t number, int64_t value) {
            if (schema.fields[number].type == UInt) {
                set(number, (uint64_t) value);
                return;
            }
            check_seq(number);
            uint64_t xval = value;
            xval = -(xval >> 63) ^ (xval << 1);
            schema.encode_type(number, _out, SInt, 0);
            schema.encode_vint(_out, xval);
            _fdset[number]=true;
        }

        void set(size_t number, uint64_t value) {
            if (schema.fields[number].type == SInt) {
                set(number, (int64_t) value);
                return;
            }
            check_seq(number);
            schema.encode_type(number, _out, UInt, 0);
            schema.encode_vint(_out, value);
            _fdset[number]=true;
        }

        void set(size_t number, int value) {
            set(number, (int64_t) value);
        }

        // output results ... if check is true, will raise on missing fields
        bytes &out(bool check=true) {
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
    };

    // cppcheck-suppress noExplicitConstructor
    Schema(const std::initializer_list<Field> &in) {        // NOLINT
        for (auto e: in) {
            add_field(e.number, e.type, e.required);
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

    Deserial decode(const bytes &value, bool check=true) const {
        size_t i = 0;
        Deserial ret(*this, value);
        while (i < value.size()) {
            Encoding enc;
            size_t num;
            decode_type(value, &enc, &num, &i);
            if (num >= size()) {
                throw Error("field number too large");
            }
            ret.decode(num, fields[num], enc, &i); 
        }
        if (check && !ret.is_valid(*this)) {
            throw Error("missing required fields");
        }
        return ret;
    }

    void add_field(size_t number, Type type, bool required) {
        if (number >= fields.size()) {
            fields.resize(number + 1);
        }
        fields[number] = {type, required};
    }

};

};
