/** TypeScript declarations for the Emscripten module built by this directory. */
declare function createQserial(options?: createQserial.ModuleOptions): Promise<createQserial.Module>;

declare namespace createQserial {
    /** Embind enums are objects, rather than JavaScript numeric enums. */
    interface TypeValue<T extends number> {
        readonly value: T;
    }
    type FieldType = TypeValue<1> | TypeValue<2> | TypeValue<3> | TypeValue<4> | TypeValue<5>;

    interface Handle {
        /** Release the C++ object. Do not use the handle after deletion. */
        delete(): void;
        isDeleted(): boolean;
    }

    interface Serial extends Handle {
        /** Fields must be written in ascending order. Integers use 32-bit conversions. */
        set(number: number, value: string | number | boolean): void;
        /** A borrowed view: copy with slice() before deleting or changing the encoder. */
        out(check: boolean): Uint8Array;
    }

    interface Deserial extends Handle {
        /** SInt may return bigint, depending on the Emscripten WASM_BIGINT setting. */
        get(number: number): string | number | bigint;
    }

    interface Schema extends Handle {
        get_type(number: number): number;
        /** All four arguments are required by Embind, including repeated. */
        add_field(number: number, type: FieldType, required: boolean, repeated: boolean): void;
        encode(): Serial;
        /** Copies the input. Keep the schema alive until the decoder is deleted. */
        decode(data: ArrayLike<number>, check: boolean): Deserial;
    }

    interface ModuleOptions {
        locateFile?: (path: string, prefix: string) => string;
        wasmBinary?: ArrayBuffer | Uint8Array;
        print?: (text: string) => void;
        printErr?: (text: string) => void;
    }

    interface Module {
        readonly Type: {
            readonly UInt: TypeValue<1>;
            readonly SInt: TypeValue<2>;
            readonly Bin: TypeValue<3>;
            readonly Dbl: TypeValue<4>;
            readonly Flt: TypeValue<5>;
        };
        readonly Schema: { new(): Schema };
        readonly "Schema.Serial": { new(schema: Schema): Serial };
        /** Decode handles are returned by Schema.decode(), not constructed directly. */
        readonly "Schema.Deserial": { readonly prototype: Deserial };
        exceptionMsg(exceptionPtr: number): string;
    }
}

export = createQserial;
