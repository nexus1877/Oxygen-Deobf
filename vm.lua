local env = require("Oxygen-Deobf.env")
local collector = require("Oxygen-Deobf.collector")

local VM = {}

function VM.create(ast)
    return env.new(ast)
end

function VM.run(vm)
    local ok, res = vm:run()
    if ok then
        collector.record(vm)
    end
    return vm
end

return VM
