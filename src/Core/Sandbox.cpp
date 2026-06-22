#include "Sandbox.hpp"
#include <stdexcept>

Sandbox::Sandbox() {
    setupSafeEnvironment();
}

void Sandbox::setupSafeEnvironment() {
    lua_State* L = vm.state();
    lua_pushnil(L); lua_setglobal(L, "dofile");
    lua_pushnil(L); lua_setglobal(L, "loadfile");
    lua_pushnil(L); lua_setglobal(L, "loadstring");
    lua_pushnil(L); lua_setglobal(L, "os");
    lua_pushnil(L); lua_setglobal(L, "io");
    lua_pushnil(L); lua_setglobal(L, "require");
    lua_pushnil(L); lua_setglobal(L, "package");
}

std::string Sandbox::dumpFunctionBytes() {
    lua_State* L = vm.state();
    lua_pushcfunction(L, [](lua_State* L) -> int {
        size_t len;
        const char* bytes = lua_tolstring(L, 1, &len);
        lua_pushlstring(L, bytes, len);
        return 1;
    }, nullptr);
    lua_getglobal(L, "string");
    lua_getfield(L, -1, "dump");
    lua_remove(L, -2);
    lua_pushvalue(L, -2);
    lua_call(L, 1, 1);
    std::string bytecode(lua_tostring(L, -1), lua_strlen(L, -1));
    lua_pop(L, 2);
    return bytecode;
}

DeobfResult Sandbox::extractFunctions(const std::string& obfuscatedSource) {
    DeobfResult result;
    vm.reset();
    setupSafeEnvironment();
    lua_State* L = vm.state();
    std::string hookScript = R"LUA(
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
    if (!vm.loadSource(hookScript, "=hook")) throw std::runtime_error("hook failed");
    vm.execute();
    lua_setglobal(L, "captureTable");
    std::string fullScript = "captureTable = (" + hookScript + ")()\n" + obfuscatedSource;
    if (!vm.loadSource(fullScript, "=deobf")) throw std::runtime_error("load failed");
    vm.execute();
    vm.getGlobal("captureTable");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        result.cleanSource = obfuscatedSource;
        return result;
    }
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        std::string name = lua_tostring(L, -2);
        if (lua_isfunction(L, -1)) {
            lua_pushvalue(L, -1);
            std::string bc = dumpFunctionBytes();
            result.cleanSource += "-- " + name + "\n" + bc + "\n";
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return result;
}
