#pragma once
#include <string>
#include <memory>
#include <lua.h>
#include <luacode.h>
#include <Luau/Compiler.h>
#include <Luau/BytecodeBuilder.h>

class LuauVM {
public:
    LuauVM();
    ~LuauVM();
    bool loadSource(const std::string& source, const std::string& chunkname = "=stdin");
    bool loadBytecode(const std::string& bytecode, const std::string& chunkname = "=bytecode");
    int execute(int nargs = 0, int nresults = 0);
    void reset();
    lua_State* state();
    void setGlobal(const std::string& name, int index);
    void getGlobal(const std::string& name);
    void pushCFunction(lua_CFunction fn, const std::string& name = "");
private:
    lua_State* L;
};
