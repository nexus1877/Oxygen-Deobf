local source = ...
local captured = {}
local env = setmetatable({}, {__index = function(_, k) return _G[k] end})
local protected = {loadstring = true, load = true, assert = true, error = true, select = true, pairs = true, ipairs = true, next = true, tostring = true, tonumber = true, type = true, unpack = true, table = true, string = true, math = true, debug = true, coroutine = true, utf8 = true, os = true, io = true, package = true, require = true, dofile = true, loadfile = true, print = true, warn = true, setmetatable = true, getmetatable = true, rawequal = true, rawget = true, rawset = true, xpcall = true, pcall = true, _G = true, _VERSION = true}

local seen = {}
local function mark(t)
    for k, v in pairs(t) do
        if type(v) == "function" then seen[v] = true end
    end
end
mark(_G)
mark(env)

local function capture(f)
    if type(f) == "function" and not seen[f] then
        captured[#captured + 1] = f
        seen[f] = true
    end
end

local function safeLoadstring(code, chunkname)
    local f, err = loadstring(code, chunkname or "=WRD")
    if not f then return nil, err end
    capture(f)
    return f
end

env.loadstring = safeLoadstring
env.load = safeLoadstring

local fn, err = loadstring(source, "@WeAreDevs")
if not fn then return source end
setfenv(fn, env)

local ok, result = pcall(fn)
if not ok then return source end

local function scan(t)
    for k, v in pairs(t) do
        if type(v) == "function" then capture(v) end
    end
end
scan(env)
scan(_G)

local bytecodes = {}
for _, f in ipairs(captured) do
    local ok, bc = pcall(string.dump, f)
    if ok and bc and #bc > 0 then
        bytecodes[#bytecodes + 1] = bc
    end
end

return #bytecodes > 0 and table.concat(bytecodes, "\n") or source
