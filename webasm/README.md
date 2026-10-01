# TypeScript bindings

`qserial.d.ts` describes the existing Emscripten API; it does not change the
serialization format. Build `qserial.js` and `qserial.wasm` in this directory:

```sh
make qserial.js
npm ci
make typescript-test
```

The default build downloads Emscripten via `get-sdk.sh`. To use an existing SDK
(for example the Emscripten 4.0.23 version used in CI):

```sh
make qserial.js EMSDK_ENV=/path/to/emsdk/emsdk_env.sh
```

Keep `qserial.d.ts`, `qserial.js`, and `qserial.wasm` together when copying them
to a project. TypeScript resolves the declarations for `import createQserial =
require("./qserial")`. The default build is a CommonJS module suitable for Node.js;
browser projects need to load the generated Emscripten module and arrange for
the `.wasm` asset to be served. `locateFile` can customize its location.

See [example.ts](example.ts) for a complete string round trip with explicit
cleanup. The core API is:

```ts
import createQserial = require("./qserial");

const qs = await createQserial();
const schema = new qs.Schema();
schema.add_field(1, qs.Type.Bin, true, false);
const encoder = schema.encode();
encoder.set(1, "hello");
const bytes = encoder.out(true).slice();
encoder.delete();
const decoder = schema.decode(bytes, true);
console.log(decoder.get(1)); // hello
decoder.delete();
schema.delete();
```

Use the snippet inside an async function for CommonJS. Prefer the `try/finally`
cleanup shown in `example.ts` in application code.

## Binding details

- `qs.Type` values are Embind objects with a numeric `.value` property. Pass the
  object to `add_field`; `get_type` returns the numeric value.
- All four `add_field` arguments are required, as are the `check` arguments to
  `out` and `decode`. C++ default arguments are not exposed by these bindings.
- `out` returns a borrowed `Uint8Array` into WebAssembly memory. Use `.slice()`
  before deleting or changing the encoder. `decode` copies its input and accepts
  typed arrays or number arrays. Keep the schema alive until its encoders and
  decoders have been deleted. Embind handles must be released with `.delete()`.
- `set` accepts strings, numbers, and booleans. Binary fields currently use
  strings; the encoder does not expose a byte-array overload.
- Integer inputs currently pass through 32-bit C++ conversions (`UInt` unsigned,
  `SInt` signed), despite the C++ library supporting 64-bit integers. `UInt`
  output is also converted to 32 bits. `SInt` output may be `number` or `bigint`
  depending on the SDK's `WASM_BIGINT` setting. The declarations therefore return
  `string | number | bigint` from `get`; narrow the result before using it.
- Write fields in ascending order. The JavaScript API has no repeated-field
  getters, even though schemas can mark fields repeated.

## Checks

`npm run typecheck` checks the declarations, example, and expected type errors
without building WebAssembly. `make typescript-test` also compiles and executes
the TypeScript test against the actual module, covering field types, a fixed
wire-format fixture, array input, and object cleanup. `make test` runs the
existing JavaScript tests. TypeScript is a development dependency only.

CI pins both GitHub Actions and the SDK installer to commit hashes. The Linux
x64 SDK and bundled Node archives are fetched from the official Emscripten
release host and checked against the reviewed SHA-256 digests in
[`.github/emsdk.sha256`](../.github/emsdk.sha256) before installation. Update the
installer, archive URLs, and digests together when upgrading the CI toolchain.
