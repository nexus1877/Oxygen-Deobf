#include "Sandbox.hpp"
#include <stdexcept>
#include <lua.h>
#include <lualib.h>
#include <luacode.h>
#include <cstring>
#include <unordered_set>
#include <vector>

static void* luau_alloc(void*, void* ptr, size_t, size_t nsize) {
    if (nsize == 0) { free(ptr); return nullptr; }
    return realloc(ptr, nsize);
}

static int safeLoader(lua_State* L) {
    size_t len;
    const char* code = luaL_checklstring(L, 1, &len);
    std::string wrapped = "local _ENV = setmetatable({},{__index=function(t,k) if k=='loadstring' or k=='load' then return nil end return _G[k] end}) ";
    wrapped += std::string(code, len);
    if (luau_load(L, "=safe", wrapped.data(), wrapped.size(), 0) != 0) {
        lua_pushnil(L);
        lua_pushvalue(L, -2);
        return 2;
    }
    return 1;
}

Sandbox::Sandbox() {
    setupSafeEnvironment();
}

void Sandbox::setupSafeEnvironment() {
    lua_State* L = vm.state();
    lua_pushnil(L); lua_setglobal(L, "dofile");
    lua_pushnil(L); lua_setglobal(L, "loadfile");
    lua_pushnil(L); lua_setglobal(L, "os");
    lua_pushnil(L); lua_setglobal(L, "io");
    lua_pushnil(L); lua_setglobal(L, "require");
    lua_pushnil(L); lua_setglobal(L, "package");
    lua_pushcfunction(L, safeLoader, "loadstring");
    lua_setglobal(L, "loadstring");
    lua_pushcfunction(L, safeLoader, "load");
    lua_setglobal(L, "load");
}

static void collectFunctions(lua_State* L, std::unordered_set<const void*>& seen, std::vector<std::string>& out) {
    lua_pushnil(L);
    while (lua_next(L, -2)) {
        if (lua_isfunction(L, -1)) {
            const void* ptr = lua_topointer(L, -1);
            if (seen.insert(ptr).second) {
                lua_pushvalue(L, -1);
                lua_pushcfunction(L, [](lua_State* L) -> int {
                    size_t len;
                    const char* bytes = lua_tolstring(L, 1, &len);
                    lua_pushlstring(L, bytes, len);
                    return 1;
                }, nullptr);
                lua_getglobal(L, "string");
                lua_getfield(L, -1, "dump");
                lua_remove(L, -2);
                lua_pushvalue(L, -3);
                lua_call(L, 1, 1);
                out.emplace_back(lua_tostring(L, -1), lua_strlen(L, -1));
                lua_pop(L, 2);
            }
        } else if (lua_istable(L, -1)) {
            collectFunctions(L, seen, out);
        }
        lua_pop(L, 1);
    }
}

DeobfResult Sandbox::extractFunctions(const std::string& obfuscatedSource) {
    DeobfResult result;
    vm.reset();
    setupSafeEnvironment();
    lua_State* L = vm.state();

    std::string wrapped = obfuscatedSource;
    int status = luau_load(L, "=deobf", wrapped.data(), wrapped.size(), 0);
    if (status != 0) {
        std::string err = lua_tostring(L, -1);
        lua_pop(L, 1);
        wrapped = "return " + obfuscatedSource;
        status = luau_load(L, "=deobf2", wrapped.data(), wrapped.size(), 0);
        if (status != 0) {
            lua_pop(L, 1);
            result.cleanSource = obfuscatedSource;
            return result;
        }
    }

    lua_newtable(L);
    lua_setglobal(L, "__captured");

    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        result.cleanSource = obfuscatedSource;
        return result;
    }

    lua_getglobal(L, "_G");
    std::unordered_set<const void*> seen;
    std::vector<std::string> funcs;
    collectFunctions(L, seen, funcs);
    lua_pop(L, 1);

    for (const auto& bc : funcs) {
        result.cleanSource += bc + "\n";
    }
    return result;
}
