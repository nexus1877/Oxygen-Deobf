local Collector = {}

function Collector.record(vm)
    vm.log = vm.log or {}
    table.insert(vm.log, { type = "vm_done", timestamp = os.clock() })
end

function Collector.get(vm)
    return vm.log or {}
end

function Collector.clear(vm)
    vm.log = {}
end

return Collector
