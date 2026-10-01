import createQserial = require("../qserial");

function assert(condition: boolean, message: string): void {
    if (!condition) throw new Error(message);
}

// Compile-time checks: these calls must be rejected, and are never executed.
function invalidCalls(qs: createQserial.Module, schema: createQserial.Schema,
                      encoder: createQserial.Serial): void {
    // @ts-expect-error Embind expects an enum object, not its numeric value.
    schema.add_field(1, 3, true, false);
    // @ts-expect-error C++ defaults do not make Embind parameters optional.
    schema.add_field(1, qs.Type.Bin, true);
    // @ts-expect-error The validation argument is required.
    encoder.out();
    // @ts-expect-error The validation argument is required.
    schema.decode(new Uint8Array());
    // @ts-expect-error The existing encoder does not accept typed arrays.
    encoder.set(1, new Uint8Array());
    // @ts-expect-error The existing encoder does not accept bigint.
    encoder.set(1, 1n);
    // @ts-expect-error Decoded fields must be narrowed before string operations.
    schema.decode([], false).get(1).toUpperCase();
}

async function main(): Promise<void> {
    const qs = await createQserial({ locateFile: (path, prefix) => prefix + path });
    const schema = new qs.Schema();
    try {
        schema.add_field(1, qs.Type.Bin, true, false);
        schema.add_field(2, qs.Type.UInt, true, false);
        schema.add_field(3, qs.Type.SInt, true, false);
        schema.add_field(4, qs.Type.Flt, true, false);
        schema.add_field(5, qs.Type.Dbl, true, false);
        assert(qs.Type.Bin.value === 3, "enum values are objects");
        assert(schema.get_type(1) === qs.Type.Bin.value, "get_type returns a number");
        const encoder = schema.encode();
        let bytes: Uint8Array;
        try {
            encoder.set(1, "hello");
            encoder.set(2, 42);
            encoder.set(3, -7);
            encoder.set(4, 1.5);
            encoder.set(5, 2.5);
            assert(encoder.out(true) instanceof Uint8Array, "output is Uint8Array");
            bytes = encoder.out(true).slice();
        } finally {
            encoder.delete();
        }
        assert(encoder.isDeleted(), "encoder was released");
        // This is a fixed C++ wire-format fixture, not just a round-trip assertion.
        assert(Array.from(bytes).join(",") ===
            "5,5,104,101,108,108,111,8,42,12,13,18,0,0,192,63,23,0,0,0,0,0,0,4,64",
            "encoded bytes match the C++ wire format");
        for (const input of [bytes, Array.from(bytes)]) {
            const decoder = schema.decode(input, true);
            try {
                assert(decoder.get(1) === "hello", "string round-trip");
                assert(decoder.get(2) === 42, "unsigned integer round-trip");
                assert(Number(decoder.get(3)) === -7, "signed integer round-trip");
                assert(decoder.get(4) === 1.5, "float round-trip");
                assert(decoder.get(5) === 2.5, "double round-trip");
            } finally {
                decoder.delete();
            }
        }
        const unchecked = schema.decode([], false);
        unchecked.delete();
    } finally {
        schema.delete();
    }
    console.log("TypeScript bindings: all checks passed");
}

main().catch(error => {
    console.error(error);
    throw error;
});
