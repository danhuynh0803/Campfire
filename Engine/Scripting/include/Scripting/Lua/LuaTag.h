#pragma once
#include <lua.hpp>

namespace LuaTag{
	int GetTag(lua_State* L);

	extern const luaL_Reg tagLib[];
}