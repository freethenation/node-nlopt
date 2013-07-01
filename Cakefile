fs = require 'fs'
ctrllib = require 'ctrl'
_ = require 'lodash'
childProcess = require 'child_process'
flatten = _.flatten
file = require 'file'
path = require 'path'
open = require 'open'
Mocha = require 'mocha'

task 'test', 'runs all unit tests', ()->
    mocha = new Mocha({
        ui: 'bdd',
        reporter: 'list'
    })
    fs.readdirSync('./test').filter((file)-> return file.substr(-3) == '.js';)
        .forEach((file)->mocha.addFile(path.join('./test', file)))
    mocha.run((failures)->process.exit(failures))

task 'build', 'builds all .coffee files', (options)->
    findCoffeeFiles(".coffee", (fileName)->
        console.log("Compiling '#{fileName}'")
        readAndCompile(fileName, (error)->
            console.log("Compiled '#{fileName}'")
            if error then console.log(error)
        )
    )

task 'build:watch', 'watches all .coffee files and rebuilds them when there are changes', (options)->
    buildCallback = (fileName, error)->
        console.log("Compiled '#{fileName}'")
        if error then console.log(error)
    findCoffeeFiles(".coffee", (fileName)->
        console.log("Watching '#{fileName}'")
        watch(fileName, buildCallback)
    )

task 'debug', 'runs the main JS file as indicated by package.js attaching node-inspector.', (options)->
    jsPath = require('./package.json').main
    jsPath = path.resolve(__dirname, jsPath)
    callback = (err, stdout, stderr)-> if err then throw err
    console.log('launching node-inspector (node-inspector must be installed globally)...')
    childProcess.exec('node-inspector', callback)
    console.log('running the js script indicated as main in package.json...')
    node = childProcess.exec("node --debug-brk \"#{jsPath}\"", callback)
    console.log('opening "http://127.0.0.1:8080/debug?port=5858" in browser...\n\n')
    setTimeout((()->open('http://127.0.0.1:8080/debug?port=5858')), 500)
    node.stdin.pipe(process.stdin);
    node.stdout.pipe(process.stdout);
    node.stderr.pipe(process.stderr);

findCoffeeFiles = (fileExt, callback)->
    file.walk('./', (n, dirPath, dirs, files)->
        for fileName in files when fileName.indexOf(fileExt) != -1 and fileName.indexOf("node_modules") == -1
            callback?(fileName)
    )

watch = (fileName, buildCallback) ->
    listener = ()->
        ctrllib([
            (ctrl)->readAndCompile(fileName, ctrl.next)
            (ctrl, error)->
                buildCallback?(fileName, error)
                ctrl.next()
        ])
    listener()
    fs.watchFile(fileName, listener)

readAndCompile = (fileName, callback)->
    ctrllib([
        (ctrl)->readFile(fileName, ctrl.next)
        (ctrl, file)->compile(file, ctrl.next)
        (ctrl, file)->writeFile(fileName.replace(".coffee",".js"), file, ctrl.next)
    ], {errorHandler:(ctrl,error)->callback?(error)}, ()->callback?())

compile = (file, callback)->
    coffee = require 'coffee-script'
    callback?(coffee.compile(file))

compressCss = (inputFile, callback) ->
    callback?(require('clean-css').process(inputFile))
    
readFile = (filename, callback) ->
    data = fs.readFile(filename, 'utf8', (err, data)-> if err then throw err else callback(data))
 
writeFile = (filename, data, callback) ->
    fs.writeFile(filename, data, 'utf8', (err)-> if err then throw err else callback())
