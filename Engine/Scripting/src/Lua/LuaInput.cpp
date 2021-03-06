#include "Scripting/Lua/LuaInput.h"
#include "Core/Input.h"

int LuaInput::GetButton(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* key = luaL_checkstring(L, topIndex);
    lua_pushboolean(L, Input::GetButton(std::string(key)));
    return 1;
}

int LuaInput::GetButtonDown(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* key = luaL_checkstring(L, topIndex);
    lua_pushboolean(L, Input::GetButtonDown(std::string(key)));
    return 1;
}

int LuaInput::GetButtonUp(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* key = luaL_checkstring(L, topIndex);
    lua_pushboolean(L, Input::GetButtonUp(std::string(key)));
    return 1;
}

int LuaInput::GetMouseButton(lua_State* L)
{
    int topIndex = lua_gettop(L);
	const char* mouseCode = luaL_checkstring(L, topIndex);
	lua_pushboolean(L, Input::GetMouseButton(std::string(mouseCode)));
	return 1;
}

int LuaInput::GetMouseButtonDown(lua_State* L)
{
    int topIndex = lua_gettop(L);
	const char* mouseCode = luaL_checkstring(L, topIndex);
	lua_pushboolean(L, Input::GetMouseButtonDown(std::string(mouseCode)));
	return 1;
}

int LuaInput::GetMouseButtonUp(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checkstring(L, topIndex);
	const char* mouseCode = lua_tostring(L, -1);
	lua_pushboolean(L, Input::GetMouseButtonUp(std::string(mouseCode)));
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
    {"GetButton",LuaInput::GetButton},
    {"GetButtonDown",LuaInput::GetButtonDown},
    {"GetButtonUp",LuaInput::GetButtonUp},
    {"GetMouseButtonDown",LuaInput::GetMouseButtonDown},
    {"GetMouseButtonUp",LuaInput::GetMouseButtonUp},
    {"GetMouseX",LuaInput::GetMouseX},
    {"GetMouseY",LuaInput::GetMouseY},
    {NULL,NULL},
};
