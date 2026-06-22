local source = ...
local env = setmetatable({}, {__index = function(t, k) if k == "loadstring" or k == "load" then return nil end end})

local fn = loadstring(source, "@WeAreDevs_Obfuscated")
if not fn then return source end

setfenv(fn, env)
local success, result = pcall(fn)
if not success then return source end

local dump = {}
local function walk(t, name)
    if type(t) == "function" then
        table.insert(dump, string.dump(t))
    end
end

for k, v in pairs(env) do
    walk(v, k)
end

return #dump > 0 and table.concat(dump, "\n") or source
