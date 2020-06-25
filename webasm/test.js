let qs = require('./qserial.js')


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
        let d = new Date();
        let t = d.toLocaleTimeString().slice(0,-3) + "." + d.getMilliseconds()
        this._log.push(["[" + t + "]", ...args])
    }
    
    add(name, func) {
        this._tests.push({"name": name, "func": func})
    }
    
    async run() {
        let failed = 0
        for (let t of this._tests) {
            this._log = []
            try {
                await t.func()
                console.log(this.color("green", "OK: "), t.name)
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
            console.log(this.color("green", "PASSED"))
        }
    }

    check(func, msg) {
        if (!func()) {
            throw Error("Check: " + msg.join(" "))
        }
    }
    isEq(a, b) {
        this.check(()=>{return a==b}, [a, "==", b])
    }
    isNeq(a, b) {
        this.check(()=>{return a!=b}, [a, "!=", b])
    }
    isTrue(a) {
        this.check(()=>{return a}, [a])
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
    test.isEq(d.get(1), "hello")
})
 
async function main() {
    qs = await qs()
    await test.run()
}

main()
