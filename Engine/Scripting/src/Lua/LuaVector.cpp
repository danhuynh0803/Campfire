#include "Scripting/Lua/LuaVector.h"
#include <glm/glm.hpp>

int LuaVector::LuaVec2(lua_State* L)
{
	//void* ptr = lua_newuserdata(L, sizeof(glm::vec2));
	//new(ptr)glm::vec2();

	lua_newtable(L);
	lua_pushstring(L, "x"); 
	lua_pushnumber(L, 0);
	lua_settable(L, -3);//might be able to be replace by lua_setfield(L,-2,name)
	lua_pushstring(L, "y");
	lua_pushnumber(L, 0);
	lua_settable(L, -3);

	luaL_getmetatable(L, "LuaVec2MetaTable");
	lua_setmetatable(L, -2);

	return 1;
}
int LuaVector::LuaVec3(lua_State* L)
{
	lua_newtable(L);
	lua_pushstring(L, "x");
	lua_pushnumber(L, 0);
	lua_settable(L, -3);
	lua_pushstring(L, "y");
	lua_pushnumber(L, 0);
	lua_settable(L, -3);
	lua_pushstring(L, "z");
	lua_pushnumber(L, 0);
	lua_settable(L, -3);

	luaL_getmetatable(L, "LuaVec3MetaTable");
	lua_setmetatable(L, -2);
	return 1;
}

int LuaVector::LuaVec4(lua_State* L)
{
	lua_newtable(L);
	lua_pushstring(L, "x");
	lua_pushnumber(L, 0);
	lua_settable(L, -3);
	lua_pushstring(L, "y");
	lua_pushnumber(L, 0);
	lua_settable(L, -3);
	lua_pushstring(L, "z");
	lua_pushnumber(L, 0);
	lua_settable(L, -3);
	lua_pushstring(L, "w");
	lua_pushnumber(L, 0);
	lua_settable(L, -4);

	luaL_getmetatable(L, "LuaVec4MetaTable");
	lua_setmetatable(L, -2);
	return 1;
}

int LuaVector::LuaVec2Add(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_pushstring(L, "x");
	lua_gettable(L, -3);
	lua_Number x1 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -3);
	lua_Number y1 = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_pushstring(L, "x");
	lua_gettable(L, -2);
	lua_Number x2 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -2);
	lua_Number y2 = lua_tonumber(L, -1);
	lua_pop(L, 1);

	LuaVec2(L);

	lua_pushstring(L, "x");
	lua_pushnumber(L, x1+x2);
	lua_rawset(L, -3);
	lua_pushstring(L, "y");
	lua_pushnumber(L, y1 + y2);
	lua_rawset(L, -3);

	return 1;
}

int LuaVector::LuaVec3Add(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_pushstring(L, "x");
	lua_gettable(L, -3);
	lua_Number x1 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -3);
	lua_Number y1 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "z");
	lua_gettable(L, -3);
	lua_Number z1 = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_pushstring(L, "x");
	lua_gettable(L, -2);
	lua_Number x2 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -2);
	lua_Number y2 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "z");
	lua_gettable(L, -2);
	lua_Number z2 = lua_tonumber(L, -1);
	lua_pop(L, 1);

	LuaVec3(L);

	lua_pushstring(L, "x");
	lua_pushnumber(L, x1 + x2);
	lua_rawset(L, -3);
	lua_pushstring(L, "y");
	lua_pushnumber(L, y1 + y2);
	lua_rawset(L, -3);
	lua_pushstring(L, "z");
	lua_pushnumber(L, z1 + z2);
	lua_rawset(L, -3);

	return 1;
}

int LuaVector::LuaVec4Add(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_pushstring(L, "x");
	lua_gettable(L, -3);
	lua_Number x1 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -3);
	lua_Number y1 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "z");
	lua_gettable(L, -3);
	lua_Number z1 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "w");
	lua_gettable(L, -3);
	lua_Number w1 = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_pushstring(L, "x");
	lua_gettable(L, -2);
	lua_Number x2 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -2);
	lua_Number y2 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "z");
	lua_gettable(L, -2);
	lua_Number z2 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "w");
	lua_gettable(L, -2);
	lua_Number w2 = lua_tonumber(L, -1);
	lua_pop(L, 1);

	LuaVec4(L);

	lua_pushstring(L, "x");
	lua_pushnumber(L, x1 + x2);
	lua_rawset(L, -3);
	lua_pushstring(L, "y");
	lua_pushnumber(L, y1 + y2);
	lua_rawset(L, -3);
	lua_pushstring(L, "z");
	lua_pushnumber(L, z1 + z2);
	lua_rawset(L, -3);
	lua_pushstring(L, "w");
	lua_pushnumber(L, w1 + w2);
	lua_rawset(L, -3);

	return 1;
}

int LuaVector::LuaVec2Sub(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_pushstring(L, "x");
	lua_gettable(L, -3);
	lua_Number x1 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -3);
	lua_Number y1 = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_pushstring(L, "x");
	lua_gettable(L, -2);
	lua_Number x2 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -2);
	lua_Number y2 = lua_tonumber(L, -1);
	lua_pop(L, 1);

	LuaVec2(L);

	lua_pushstring(L, "x");
	lua_pushnumber(L, x1 - x2);
	lua_rawset(L, -3);
	lua_pushstring(L, "y");
	lua_pushnumber(L, y1 - y2);
	lua_rawset(L, -3);

	return 1;
}

int LuaVector::LuaVec3Sub(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_pushstring(L, "x");
	lua_gettable(L, -3);
	lua_Number x1 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -3);
	lua_Number y1 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "z");
	lua_gettable(L, -3);
	lua_Number z1 = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_pushstring(L, "x");
	lua_gettable(L, -2);
	lua_Number x2 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -2);
	lua_Number y2 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "z");
	lua_gettable(L, -2);
	lua_Number z2 = lua_tonumber(L, -1);
	lua_pop(L, 1);

	LuaVec3(L);

	lua_pushstring(L, "x");
	lua_pushnumber(L, x1 - x2);
	lua_rawset(L, -3);
	lua_pushstring(L, "y");
	lua_pushnumber(L, y1 - y2);
	lua_rawset(L, -3);
	lua_pushstring(L, "z");
	lua_pushnumber(L, z1 - z2);
	lua_rawset(L, -3);

	return 1;
}

int LuaVector::LuaVec4Sub(lua_State* L)
{
	luaL_checktype(L, 1, LUA_TTABLE);
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_pushstring(L, "x");
	lua_gettable(L, -3);
	lua_Number x1 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -3);
	lua_Number y1 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "z");
	lua_gettable(L, -3);
	lua_Number z1 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "w");
	lua_gettable(L, -3);
	lua_Number w1 = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_pushstring(L, "x");
	lua_gettable(L, -2);
	lua_Number x2 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "y");
	lua_gettable(L, -2);
	lua_Number y2 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "z");
	lua_gettable(L, -2);
	lua_Number z2 = lua_tonumber(L, -1);
	lua_pop(L, 1);
	lua_pushstring(L, "w");
	lua_gettable(L, -2);
	lua_Number w2 = lua_tonumber(L, -1);
	lua_pop(L, 1);

	LuaVec4(L);

	lua_pushstring(L, "x");
	lua_pushnumber(L, x1 - x2);
	lua_rawset(L, -3);
	lua_pushstring(L, "y");
	lua_pushnumber(L, y1 - y2);
	lua_rawset(L, -3);
	lua_pushstring(L, "z");
	lua_pushnumber(L, z1 - z2);
	lua_rawset(L, -3);
	lua_pushstring(L, "w");
	lua_pushnumber(L, w1 - w2);
	lua_rawset(L, -3);

	return 1;
}