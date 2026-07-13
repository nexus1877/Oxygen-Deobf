local TableCollector = {}

function TableCollector.collect(data)
    local result = {}
    for _, entry in ipairs(data) do
        if entry.type == "write" and entry.value and type(entry.value) == "table" then
            result[entry.key] = entry.value
        end
    end
    return result
end

return TableCollector
