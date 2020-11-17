#include "Scripting/Lua/LuaInput.h"
#include "Core/Input.h"

int LuaInput::GetKey(lua_State* L)
{
	const char* key = lua_tostring(L, -1);
	//lua_pushboolean(L, Input::GetKey(std::string(key)));
	return 1;
}
int LuaInput::GetKeyDown(lua_State* L)
{
	const char* key = lua_tostring(L, -1);
	//lua_pushboolean(L, Input::GetKeyDown(std::string(key)));
	return 1;
}
int LuaInput::GetKeyUp(lua_State* L)
{
	const char* key = lua_tostring(L, -1);
	//lua_pushboolean(L, Input::GetKeyUp(std::string(key)));
	return 1;
}
int LuaInput::GetMouseButton(lua_State* L)
{
	const char* mouseCode = lua_tostring(L, -1);
	//lua_pushboolean(L, Input::GetMouseButton(std::string(mouseCode)));
	return 1;
}
int LuaInput::GetMouseButtonDown(lua_State* L)
{
	const char* mouseCode = lua_tostring(L, -1);
	//lua_pushboolean(L, Input::GetMouseButton(std::string(mouseCode)));
	return 1;
}
int LuaInput::GetMouseButtonUp(lua_State* L)
{
	const char* mouseCode = lua_tostring(L, -1);
	//lua_pushboolean(L, Input::GetMouseButton(std::string(mouseCode)));
	return 1;
}
int LuaInput::GetMouseX(lua_State* L)
{
	Input* input = (Input*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushnumber(L, Input::GetMouseX());
	return 1;
}
int LuaInput::GetMouseY(lua_State* L)
{
	Input* input = (Input*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushnumber(L, Input::GetMouseY());
	return 1;
}

const luaL_Reg LuaInput::inputLib[]
{
	{"GetKey",LuaInput::GetKey},
	{"GetKeyDown",LuaInput::GetKeyDown},
	{"GetKeyUp",LuaInput::GetKeyUp},
	{"GetMouseButtonDown",LuaInput::GetMouseButtonDown},
	{"GetMouseButtonUp",LuaInput::GetMouseButtonUp},
	{"GetMouseX",LuaInput::GetMouseX},
	{"GetMouseY",LuaInput::GetMouseY},
	{NULL,NULL}
};