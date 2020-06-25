let qs = require('./qserial.js')
let assert = require('assert')
let test = require('./testman.js')

const sleep = (milliseconds) => {
  return new Promise(resolve => setTimeout(resolve, milliseconds))
}

test.translateError = (e) => {
    if (typeof(e) == "number") {
        return qs.exceptionMsg(e)
    } else {
        return e
    }
}

test.add("basic", async (ctx) => {
    let s = new qs.Schema
    ctx.log("OK 0", qs.Schema)
    s.add_field(1, qs.Type.Bin, true, false)
    ctx.log("OK 1")
    let e = s.encode()
    ctx.log("OK 2")
    e.set(1, "hello")
    let res = e.out(true)
    ctx.log("OK 3", Buffer.from(res).toString('hex'))
    let d = s.decode(res, true)
    ctx.log("decoded", d)
    assert.equal(d.get(1), "hello")
})

test.add("zero", async (ctx) => {
    let s = new qs.Schema
    s.add_field(9, qs.Type.UInt, true, false)
    let e = s.encode()
    e.set(9, 0)
    let res = e.out(true)
    let d = s.decode(res, true)
    assert.equal(d.get(9), 0)
    assert.equal(Buffer.from(res).toString('hex'), "2400")
})

test.add("flt", async () => {
    let s = new qs.Schema
    s.add_field(9, qs.Type.Flt, true, false)
    s.add_field(10, qs.Type.Dbl, true, false)
    let e = s.encode()
    e.set(9, 1.5)
    e.set(10, 1.5)
    let res = e.out(true)
    let d = s.decode(res, true)
    assert.equal(d.get(9), 1.5)
    assert.equal(d.get(10), 1.5)
})

test.beforeAll = async () => {qs = await qs()}

test.run()
