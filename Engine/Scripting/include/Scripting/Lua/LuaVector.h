#pragma once

#include <lua.hpp>

namespace LuaVector
{
	int LuaVec2(lua_State*);
	int LuaVec3(lua_State* L);
	int LuaVec4(lua_State* L);
	int LuaVec2Add(lua_State* L);
	int LuaVec3Add(lua_State* L);
	int LuaVec4Add(lua_State* L);
	int LuaVec2Sub(lua_State* L);
	int LuaVec3Sub(lua_State* L);
	int LuaVec4Sub(lua_State* L);
}
