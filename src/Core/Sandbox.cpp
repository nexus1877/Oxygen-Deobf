#include "Sandbox.hpp"
#include <stdexcept>
#include <lua.h>
#include <lualib.h>
#include <luacode.h>
#include <unordered_set>
#include <iostream>

static void* luau_alloc(void*, void* ptr, size_t, size_t nsize) {
    if (nsize == 0) { free(ptr); return nullptr; }
    return realloc(ptr, nsize);
}

Sandbox::Sandbox() { setupSafeEnvironment(); }

void Sandbox::setupSafeEnvironment() {
    lua_State* L = vm.state();
    lua_pushnil(L); lua_setglobal(L, "dofile");
    lua_pushnil(L); lua_setglobal(L, "loadfile");
    lua_pushnil(L); lua_setglobal(L, "os");
    lua_pushnil(L); lua_setglobal(L, "io");
    lua_pushnil(L); lua_setglobal(L, "require");
    lua_pushnil(L); lua_setglobal(L, "package");
}

void Sandbox::collectGlobalFunctions(std::vector<std::string>& out) {
    lua_State* L = vm.state();
    lua_getglobal(L, "_G");
    std::unordered_set<const void*> seen;
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
        }
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

DeobfResult Sandbox::extractAllDefinedFunctions(const std::string& obfuscatedSource) {
    DeobfResult result;
    vm.reset();
    setupSafeEnvironment();
    lua_State* L = vm.state();

    std::vector<std::string> before;
    collectGlobalFunctions(before);

    std::string wrapped = obfuscatedSource;
    int status = luau_load(L, "=deobf", wrapped.data(), wrapped.size(), 0);
    if (status != 0) {
        std::cerr << "[Sandbox] load error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        wrapped = "return " + obfuscatedSource;
        status = luau_load(L, "=deobf2", wrapped.data(), wrapped.size(), 0);
        if (status != 0) {
            std::cerr << "[Sandbox] load error (retry): " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1);
            result.cleanSource = obfuscatedSource;
            return result;
        }
    }

    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
        std::cerr << "[Sandbox] runtime error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        result.cleanSource = obfuscatedSource;
        return result;
    }

    if (lua_isfunction(L, -1)) {
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
        result.cleanSource = std::string(lua_tostring(L, -1), lua_strlen(L, -1));
        lua_pop(L, 2);
    }

    std::vector<std::string> after;
    collectGlobalFunctions(after);

    for (size_t i = before.size(); i < after.size(); ++i) {
        result.cleanSource += after[i] + "\n";
    }

    return result;
}

DeobfResult Sandbox::extractFunctions(const std::string& obfuscatedSource) {
    return extractAllDefinedFunctions(obfuscatedSource);
}
