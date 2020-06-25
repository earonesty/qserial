let qs = require('./qserial.js')
let assert = require('assert')

class TestMan {
    constructor() {
        this._log = []
        this._tests = []
        this._color = {
            "reset" : "\x1b[0m",
            "green" : "\x1b[32m",
            "red": "\x1b[31m",
        }
    }

    color(name, str) {
        return this._color[name] + str + this._color.reset
    }

    getErr() {
        try { throw Error('') } catch(err) { return err; }
    }

    log(...args) {
        let err = this.getErr();
        let frame = err.stack.split("\n")[3];
        let lineNumber = frame.split(":")[1];
        let filePath = frame.split(":")[0].split('(')[1];
        let fileName = filePath.replace(/\\/g, '/').split('/')
        fileName = fileName[fileName.length - 1]
        let fileInfo = fileName + ":" + lineNumber
        let d = new Date();
        let t = d.toLocaleTimeString().slice(0,-3) + "." + d.getMilliseconds()
        this._log.push(["[" + t + "]", fileInfo, ...args])
    }
    
    add(name, func) {
        this._tests.push({"name": name, "func": func})
    }
    
    async run(tl) {
        let failed = 0
        let passed = 0
        let regex = new RegExp(tl.join("|"))

        for (let t of this._tests) {
            this._log = []
            if (tl) {
                if (!t.name.match(regex)) {
                    continue
                }
            }
            try {
                await t.func()
                console.log(this.color("green", "OK: "), t.name)
                passed += 1
            } catch (e) {
                failed += 1
                if (this.translateError) {
                    e = this.translateError(e)
                }
                console.log(this.color("red", "FAIL: "), t.name, "# ", e)
                for (let ent of this._log) {
                    ent.unshift("   ")
                    console.log.apply(null, ent)
                }
            }
        }
        if (!failed) {
            if (!passed) {
                console.log("No tests run.")
            } else {
                console.log(this.color("green", ":PASS:"))
            }
        }
    }
};

test = new TestMan()

test.translateError = (e) => {
    if (typeof(e) == "number") {
        return qs.exceptionMsg(e)
    } else {
        return e
    }
}

test.add("basic", async () => {
    let s = new qs.Schema
    test.log("OK 0", qs.Schema)
    s.add_field(1, qs.Type.Bin, true, false)
    test.log("OK 1")
    e = s.encode()
    test.log("OK 2")
    e.set(1, "hello")
    res = e.out(true)
    test.log("OK 3", Buffer.from(res).toString('hex'))
    d = s.decode(res, true)
    test.log("decoded", d)
    assert.equal(d.get(1), "hello")
})

test.add("zero", async () => {
    let s = new qs.Schema
    s.add_field(9, qs.Type.UInt, true, false)
    e = s.encode()
    e.set(9, 0)
    res = e.out(true)
    d = s.decode(res, true)
    assert.equal(d.get(9), 0)
    assert.equal(Buffer.from(res).toString('hex'), "2400")
})

test.add("flt", async () => {
    let s = new qs.Schema
    s.add_field(9, qs.Type.Flt, true, false)
    s.add_field(10, qs.Type.Dbl, true, false)
    e = s.encode()
    e.set(9, 1.5)
    e.set(10, 1.5)
    res = e.out(true)
    d = s.decode(res, true)
    assert.equal(d.get(9), 1.5)
    assert.equal(d.get(10), 1.5)
})

async function main() {
    let argv = process.argv
    tl = []
    for (let i=0; i < argv.length - 1; ++i) {
        if (argv[i] == "-t") {
            tl.push(argv[i+1])
        }
    }
    qs = await qs()
    await test.run(tl)
}

main()
