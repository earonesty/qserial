/*
 * Very simple test runner for nodejs:
 *
 * Supports:
 *
 *    before, after, beforeAll, afterAll
 *    fixture object passed to each test, that before/after/beforeAll/afterAll can modify
 *    -[t]est option on command line to pick tests to run
 *    -[l]inear option on command to disable parallel
 *    built in fixture logger, captures log lines, adds line numbers/file names/timestamps
 *
 * Synopsis:
 *    test = require('testman')
 *    assert = require('assert')
 *
 *    test.add("hello", ()=>{
 *      assert.equals(1,1)
 *    })
 *
 *    test.run()
 *
 */

class TestMan {
    constructor() {
        this._tests = []
        this._color = {
            "reset" : "\x1b[0m",
            "green" : "\x1b[32m",
            "red": "\x1b[31m",
        }
        this.fixtures = {}
    }

    color(name, str) {
        return this._color[name] + str + this._color.reset
    }

    getErr() {
        try { throw Error('') } catch(err) { return err; }
    }

    _logTo(logLines, ...args) {
        let err = this.getErr();
        let frame = err.stack.split("\n")[4];
        let lineNumber = frame.split(":")[1];
        let filePath = frame.split(":")[0].split('(')[1];
        let fileName = filePath.replace(/\\/g, '/').split('/')
        fileName = fileName[fileName.length - 1]
        let fileInfo = fileName + ":" + lineNumber
        let d = new Date();
        let t = d.toLocaleTimeString().slice(0,-3) + "." + d.getMilliseconds()
        logLines.push(["[" + t + "]", fileInfo, ...args])
    }
    
    add(name, func) {
        this._tests.push({"name": name, "func": func})
    }
    
    async _run_test(t, fixtures, ctx) {
        let ok
        let logLines = []

        let local = {...fixtures}

        local.log = (...args) => {
            this._logTo(logLines, ...args)
        }

        try {
            if (this.before) 
                await this.before(local)
            await t.func(local)
            console.log(this.color("green", "OK: "), t.name)
            ok = true
        } catch (e) {
            ok = false
            if (this.translateError) {
                e = this.translateError(e)
            }
            console.log(this.color("red", "FAIL: "), t.name, "# ", e)
            for (let ent of logLines) {
                ent.unshift("   ")
                console.log.apply(null, ent)
            }
        }
        if (this.after) 
            await this.after(local)
        if (ok) ctx.passed += 1
        if (!ok) ctx.failed += 1
    }
    
    async _run(tl, fixtures, parallel) {
        let ctx = {
            passed: 0,
            failed: 0
        }
        let regex = new RegExp(tl.join("|"))

        let tests = []
        for (let t of this._tests) {
            if (tl) {
                if (!t.name.match(regex)) {
                    continue
                }
            }
            let promise = this._run_test(t, fixtures, ctx)
            if (!parallel)
                await promise
            else
                tests.push(promise)
        }
        await Promise.all(tests)
        if (!ctx.failed) {
            if (!ctx.passed) {
                process.exitCode = 2
                console.log("No tests run.")
            } else {
                process.exitCode = 0
            }
        } else {
            process.exitCode = 1
        }
    }

    async run() {
        if (this.beforeAll) 
            await this.beforeAll(this.fixtures)

        let tl = []
        let parallel = true
        
        if (process) {
            let argv = process.argv
            for (let i=0; i < argv.length; ++i) {
                if (argv[i] == "-t" || argv[i] == "-test") {
                    tl.push(argv[i+1])
                }
                if (argv[i] == "-l" || argv[i] == "-linear") {
                    parallel = false
                }
            }
        }

        await this._run(tl, this.fixtures, parallel)
        
        if (this.afterAll) 
            await this.afterAll(this.fixtures)
    }

    sleep(milliseconds) {
      return new Promise(resolve => setTimeout(resolve, milliseconds))
    }
}

module.exports = new TestMan()
