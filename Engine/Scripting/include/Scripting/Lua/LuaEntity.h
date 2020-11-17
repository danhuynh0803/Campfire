#pragma once
#include <lua.hpp>

namespace LuaEntity
{
	int SetEntityPosition(lua_State*);
	int test(lua_State *);
	extern const luaL_Reg entityTransformLib[];
}
