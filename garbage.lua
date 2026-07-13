local dictionary = require("env/dict.json")
local patterns = {
    ["[%z\1-\31\127-\255]"] = true,
    ["%s%s%s+"] = true,
    ["%d%d%d%d%d%d%d%d%d%d"] = true,
}

local function isGarbage(str)
    if #str < 4 or #str > 1000 then return true end
    if str:match("^[%d_]+$") then return true end
    for pat in pairs(patterns) do
        if str:find(pat) then return true end
    end
    if dictionary[str:lower()] then return false end
    return false
end

function garbage.filter(ast)
    local function walk(node)
        if node.tag == "string" and isGarbage(node.text) then
            node.text = "<filtered>"
        end
        for k, v in pairs(node) do
            if type(v) == "table" then walk(v) end
        end
    end
    walk(ast.root)
    return ast
end

return garbage
