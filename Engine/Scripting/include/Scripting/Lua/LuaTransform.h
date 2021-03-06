#pragma once
#include <lua.hpp>

namespace LuaTransfrom {
	//int NewTransform(lua_State*);
	//int SetPosition(lua_State*);
	//int SetRotation(lua_State*);
	//int SetScale(lua_State*);
	//int GetPosition(lua_State*);
	//int GetRotation(lua_State*);
	//int GetScale(lua_State*);
	//int LuaTransformTableIndex(lua_State*);
	int SetEntityPosition(lua_State* L);
	int SetEntityRotation(lua_State* L);
	int SetEntityScale(lua_State* L);
	int GetEntityPosition(lua_State* L);
	int GetEntityRotation(lua_State* L);
	int GetEntityScale(lua_State* L);
	int Translate(lua_State* L);

	//extern const luaL_Reg transformLib[];
	extern const luaL_Reg entityTransformLib[];
}
