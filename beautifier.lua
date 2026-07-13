local Beautifier = {}

function Beautifier.process(data)
    local function pretty(t, indent)
        indent = indent or 0
        local spaces = string.rep("  ", indent)
        if type(t) ~= "table" then return tostring(t) end
        local parts = {}
        for k, v in pairs(t) do
            if type(v) == "table" then
                parts[#parts+1] = spaces .. tostring(k) .. " = " .. pretty(v, indent+1)
            else
                parts[#parts+1] = spaces .. tostring(k) .. " = " .. tostring(v)
            end
        end
        return "{\n" .. table.concat(parts, ",\n") .. "\n" .. spaces:sub(1, -3) .. "}"
    end
    return pretty(data)
end

return Beautifier
