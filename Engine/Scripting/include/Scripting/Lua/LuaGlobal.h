#pragma once
#include "lua.hpp"

namespace LuaGlobal
{
	int SetNumber(lua_State*);
	int SetBoolean(lua_State*);
	int SetInteger(lua_State*);
	int SetString(lua_State*);
	int SetTable(lua_State*);
	int GetNumber(lua_State*);
	int GetBoolean(lua_State*);
	int GetInteger(lua_State*);
	int GetString(lua_State*);
	int GetTable(lua_State*);
}