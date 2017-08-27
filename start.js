const { execSync, exec } = require('child_process')
execSync('cd server && ../node_modules/.bin/bsb -make-world -backend native')
exec('cd server && ./lib/bs/native/main.native', console.log)

// fix nodemon
process.on('exit', () => process.exit())
