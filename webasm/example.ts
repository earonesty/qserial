import createQserial = require("./qserial");

/** Demonstrate a typed string round trip while releasing every native handle. */
async function main(): Promise<void> {
    const qs = await createQserial();
    const schema = new qs.Schema();
    try {
        schema.add_field(1, qs.Type.Bin, true, false);
        const encoder = schema.encode();
        let bytes: Uint8Array;
        try {
            encoder.set(1, "hello");
            bytes = encoder.out(true).slice();
        } finally {
            encoder.delete();
        }
        const decoder = schema.decode(bytes, true);
        try {
            console.log(decoder.get(1)); // hello
        } finally {
            decoder.delete();
        }
    } finally {
        schema.delete();
    }
}

main().catch(error => {
    console.error(error);
    throw error;
});
