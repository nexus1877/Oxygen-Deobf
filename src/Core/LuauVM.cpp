#include "LuauVM.hpp"
#include <stdexcept>

static void* luau_alloc(void* ud, void* ptr, size_t osize, size_t nsize) {
    if (nsize == 0) {
        free(ptr);
        return NULL;
    }
    return realloc(ptr, nsize);
}

LuauVM::LuauVM() {
    L = lua_newstate(luau_alloc, NULL);
    luaL_openlibs(L);
}

LuauVM::~LuauVM() {
    if (L) lua_close(L);
}

bool LuauVM::loadSource(const std::string& source, const std::string& chunkname) {
    std::string bytecode = Luau::compile(source);
    return loadBytecode(bytecode, chunkname);
}

bool LuauVM::loadBytecode(const std::string& bytecode, const std::string& chunkname) {
    int status = luau_load(L, chunkname.c_str(), bytecode.data(), bytecode.size(), 0);
    return status == 0;
}

int LuauVM::execute(int nargs, int nresults) {
    int status = lua_pcall(L, nargs, nresults, 0);
    if (status != LUA_OK) {
        std::string err = lua_tostring(L, -1);
        lua_pop(L, 1);
        throw std::runtime_error(err);
    }
    return lua_gettop(L);
}

void LuauVM::reset() {
    if (L) lua_close(L);
    L = lua_newstate(luau_alloc, NULL);
    luaL_openlibs(L);
}

lua_State* LuauVM::state() { return L; }

void LuauVM::setGlobal(const std::string& name, int index) {
    lua_setglobal(L, name.c_str());
}

void LuauVM::getGlobal(const std::string& name) {
    lua_getglobal(L, name.c_str());
}

void LuauVM::pushCFunction(lua_CFunction fn, const std::string& name) {
    lua_pushcfunction(L, fn, name.c_str());
}
