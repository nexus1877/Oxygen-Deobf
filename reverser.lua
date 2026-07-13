local Reverser = {}

function Reverser.process(log)
    local reversed = {}
    for _, entry in ipairs(log) do
        if entry.type == "call" then
            local func = entry.func
            if type(func) == "function" then
                local info = debug.getinfo(func)
                if info and info.source then
                    reversed[#reversed+1] = {
                        type = "function",
                        source = info.source,
                        args = entry.args,
                        path = entry.path
                    }
                end
            end
        elseif entry.type == "read" or entry.type == "write" then
            reversed[#reversed+1] = {
                type = entry.type,
                key = entry.key,
                value = entry.value,
                path = entry.path
            }
        end
    end
    return reversed
end

return Reverser
