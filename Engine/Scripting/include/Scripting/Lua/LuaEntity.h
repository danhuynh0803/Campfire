#pragma once
#include <lua.hpp>

namespace LuaEntity
{
	int SetEntityPosition(lua_State*);
	int SetEntityRotation(lua_State* L);
	int SetEntityScale(lua_State* L);
	extern const luaL_Reg entityTransformLib[];
}