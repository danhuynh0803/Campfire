#include "Scripting/Lua/LuaGlobal.h"
#include "Scripting/LuaManager.h"

int LuaGlobal::SetNumber(lua_State* L)
{
    int topIndex = lua_gettop(L);
    lua_Number nunber = luaL_checknumber(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex - 1);
    return 0;
}

int LuaGlobal::SetBoolean(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checktype(L, topIndex, LUA_TBOOLEAN);
    bool value = lua_toboolean(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex - 1);
    return 0;
}

int LuaGlobal::SetInteger(lua_State* L)
{
    int topIndex = lua_gettop(L);
    int nunber = luaL_checkinteger(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex - 1);
    return 0;
}

int LuaGlobal::SetString(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* value = luaL_checkstring(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex - 1);
    return 0;
}

int LuaGlobal::SetTable(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checktype(L, topIndex, LUA_TTABLE);
    const char* name = luaL_checkstring(L, topIndex - 1);
    return 0;
}

int LuaGlobal::GetNumber(lua_State* L)
{
    int topIndex = lua_gettop(L);
    lua_Number nunber = luaL_checknumber(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex);
    return 1;
}

int LuaGlobal::GetBoolean(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checktype(L, topIndex, LUA_TBOOLEAN);
    bool value = lua_toboolean(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex);
    return 1;
}

int LuaGlobal::GetInteger(lua_State* L)
{
    int topIndex = lua_gettop(L);
    int nunber = luaL_checkinteger(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex);
    return 1;
}

int LuaGlobal::GetString(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    return 1;
}

int LuaGlobal::GetTable(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    return 1;
}