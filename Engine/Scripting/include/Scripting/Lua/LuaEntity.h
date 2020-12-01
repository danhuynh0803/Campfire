#pragma once
#include <lua.hpp>

namespace LuaEntity
{
    struct LuaEntity {

    };
    int Instantiate(lua_State*);
    int EntityDestroy(lua_State*);
    int OtherEntityDestroy(lua_State*);
    extern const luaL_Reg entityLib[];
}
