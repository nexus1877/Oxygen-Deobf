local hooks = require("Oxygen-Deobf.hooks")
local utils = require("Oxygen-Deobf.utils")

local Env = {}
Env.__index = Env

function Env.new(ast)
    local self = setmetatable({}, Env)
    self.ast = ast
    self.env = { _G = {} }
    self.log = {}
    return self
end

function Env:run()
    local code = self:generateCode(self.ast.root)
    local func = loadstring(code, "=OxygenVM")
    if not func then return false end
    setfenv(func, self.env)
    hooks.install(self.env, self.log)
    local ok, res = pcall(func)
    if ok then
        self.log = hooks.collect(self.env)
    end
    return ok, res
end

function Env:generateCode(node)
    if node.tag == "block" then
        local parts = {}
        for _, stat in ipairs(node.statements) do
            parts[#parts+1] = self:generateCode(stat)
        end
        return table.concat(parts, ";")
    elseif node.tag == "assign" then
        local vars = {}
        for _, v in ipairs(node.variables) do vars[#vars+1] = v.node.name.text end
        local vals = {}
        for _, v in ipairs(node.values) do vals[#vals+1] = self:generateCode(v.node) end
        return table.concat(vars, ",") .. "=" .. table.concat(vals, ",")
    elseif node.tag == "call" then
        local args = {}
        for _, a in ipairs(node.arguments) do args[#args+1] = self:generateCode(a.node) end
        return self:generateCode(node.func) .. "(" .. table.concat(args, ",") .. ")"
    elseif node.tag == "number" then
        return tostring(node.value)
    elseif node.tag == "string" then
        return string.format("%q", node.text)
    elseif node.tag == "boolean" then
        return tostring(node.value)
    elseif node.tag == "nil" then
        return "nil"
    elseif node.tag == "global" or node.tag == "local" then
        return node.name.text
    elseif node.tag == "index" then
        return self:generateCode(node.expression) .. "[" .. self:generateCode(node.index) .. "]"
    elseif node.tag == "indexname" then
        return self:generateCode(node.expression) .. "." .. node.index.text
    elseif node.tag == "function" then
        local params = {}
        for _, p in ipairs(node.body.parameters) do params[#params+1] = p.text end
        local body = self:generateCode(node.body.body)
        return "function(" .. table.concat(params, ",") .. ")" .. body .. "end"
    else
        return ""
    end
end

return Env
