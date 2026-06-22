#pragma once
#include <string>
inline const std::string HOOK_SCRIPT = R"LUA(
    local captured = {}
    local old_pairs = pairs
    local function sniff()
        for k,v in old_pairs(_G) do
            if type(v) == "function" and not captured[k] then
                captured[k] = v
            end
        end
    end
    sniff()
    setmetatable(_G, {
        __newindex = function(t,k,v)
            rawset(t,k,v)
            if type(v) == "function" then captured[k] = v end
        end
    })
    return setmetatable({}, {
        __index = function(_, k)
            return captured[k]
        end
    })
)LUA";
