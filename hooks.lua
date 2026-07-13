local collected = {}

local function hookFunction(func, log, path)
    return function(...)
        local args = {...}
        log[#log+1] = { type = "call", func = func, args = args, path = path }
        return func(...)
    end
end

local function hookTable(t, log, path)
    return setmetatable({}, {
        __index = function(self, k)
            log[#log+1] = { type = "read", table = t, key = k, path = path }
            return t[k]
        end,
        __newindex = function(self, k, v)
            log[#log+1] = { type = "write", table = t, key = k, value = v, path = path }
            t[k] = v
        end,
        __call = function(self, ...)
            log[#log+1] = { type = "call", table = self, args = {...}, path = path }
        end
    })
end

function hooks.install(env, log)
    local function traverse(t, path)
        for k, v in pairs(t) do
            local newPath = path .. "." .. tostring(k)
            if type(v) == "function" then
                t[k] = hookFunction(v, log, newPath)
            elseif type(v) == "table" then
                t[k] = hookTable(v, log, newPath)
                traverse(t[k], newPath)
            end
        end
    end
    traverse(env, "env")
    return env
end

function hooks.collect(env)
    return collected
end

return hooks
