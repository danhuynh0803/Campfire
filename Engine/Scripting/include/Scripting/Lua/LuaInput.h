#pragma once
#include <lua.hpp>

namespace LuaInput
{
	int GetKey(lua_State*);
	int GetKeyDown(lua_State*);
	int GetKeyUp(lua_State*);
	int GetMouseButton(lua_State*);
	int GetMouseButtonDown(lua_State*);
	int GetMouseButtonUp(lua_State*);
	int GetMouseX(lua_State*);
	int GetMouseY(lua_State*);
	
	extern const luaL_Reg LuaInput[];
}