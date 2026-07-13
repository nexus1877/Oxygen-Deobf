local ast = require("Oxygen-Deobf.ast")
local vm = require("Oxygen-Deobf.vm")
local collector = require("Oxygen-Deobf.collector")
local reverser = require("Oxygen-Deobf.reverser")
local beautifier = require("Oxygen-Deobf.beautifier")
local tableCollector = require("Oxygen-Deobf.table_collector")
local sender = require("Oxygen-Deobf.sender")
local config = require("Oxygen-Deobf.config")

local function deobfuscate(source)
    local parsed = ast.parse(source)
    local env = vm.create(parsed)
    vm.run(env)
    local runtimeData = collector.get(env)
    local reversed = reverser.process(runtimeData)
    local beautified = beautifier.process(reversed)
    local finalTable = tableCollector.collect(beautified)
    return sender.send(finalTable)
end

return { deobfuscate = deobfuscate }
