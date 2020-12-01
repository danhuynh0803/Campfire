#pragma once
#include "lua.hpp"

namespace LuaCall {
	int LuaCallback(lua_State*);
	int Start(lua_State* L);
	int Update(lua_State* L);
	int OnTriggerEnter(lua_State*);
	int OnTriggerStay(lua_State* L);
	int OnTriggerExit(lua_State* L);
}