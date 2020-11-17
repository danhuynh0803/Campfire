#pragma once
#include <lua.hpp>

namespace LuaTransfrom {
	int NewTransform(lua_State*);
	int SetPosition(lua_State*);
	int SetRotation(lua_State*);
	int SetScale(lua_State*);
	int GetPosition(lua_State*);
	int GetRotation(lua_State*);
	int GetScale(lua_State*);
	int LuaTransformTableIndex(lua_State*);

	extern const luaL_Reg transformLib[];
}
