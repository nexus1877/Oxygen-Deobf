#include "LuauVM.hpp"
#include <stdexcept>

LuauVM::LuauVM() {
    L = luaL_newstate();
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
    lua_close(L);
    L = luaL_newstate();
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
