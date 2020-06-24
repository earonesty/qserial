let qs = require('./qserial.js')

async function main() {
    qs = await qs()
    let s = new qs.Schema
    s.add_field(0, 1, false, false)
    console.log("stuff worked")
}

main()
