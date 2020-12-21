#include "Scripting/Lua/LuaGlobal.h"
#include "Scripting/LuaManager.h"

int LuaGlobal::SetNumber(lua_State* L)
{
    int topIndex = lua_gettop(L);
    lua_Number number = luaL_checknumber(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex - 1);
    LuaManager::SetGlobalLuaNumber(name, number);
    return 0;
}

int LuaGlobal::SetBoolean(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checktype(L, topIndex, LUA_TBOOLEAN);
    bool value = lua_toboolean(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex - 1);
    LuaManager::SetGlobalLuaBoolean(name, value);
    return 0;
}

int LuaGlobal::SetInteger(lua_State* L)
{
    int topIndex = lua_gettop(L);
    lua_Integer nunber = luaL_checkinteger(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex - 1);
    LuaManager::SetGlobalLuaInteger(name, nunber);
    return 0;
}

int LuaGlobal::SetString(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* value = luaL_checkstring(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex - 1);
    LuaManager::SetGlobalLuaString(name, value);
    return 0;
}

int LuaGlobal::SetTable(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checktype(L, topIndex, LUA_TTABLE);
    const char* name = luaL_checkstring(L, topIndex - 1);
    if (!LuaManager::SetGlobalLuaTable(name, L))
    {
        lua_pushstring(L, "Cannot set the table value.");
        lua_error(L);
    }
    return 0;
}

int LuaGlobal::GetNumber(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    lua_Number number;
    if (!LuaManager::GetGlobalLuaNumber(name, number))
    {
        lua_pushstring(L, "Cannot find the double value.");
        lua_error(L);
    }
    lua_pushboolean(L, number);
    return 1;
}

int LuaGlobal::GetBoolean(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    bool value;
    if (!LuaManager::GetGlobalLuaBoolean(name,value))
    {
        lua_pushstring(L, "Cannot find the boolean value.");
        lua_error(L);
    }
    lua_pushboolean(L, value);
    return 1;
}

int LuaGlobal::GetInteger(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    lua_Integer number;
    if (!LuaManager::GetGlobalLuaInteger(name, number))
    {
        lua_pushstring(L, "Cannot find the integer value.");
        lua_error(L);
    }
    lua_pushinteger(L, number);
    return 1;
}

int LuaGlobal::GetString(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    const char* value;
    if (!LuaManager::GetGlobalLuaString(name, value))
    {
        lua_pushstring(L, "Cannot find the integer value.");
        lua_error(L);
    }
    lua_pushstring(L, value);
    return 1;
}

int LuaGlobal::GetTable(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    if (!LuaManager::GetGlobalLuaTable(name, L))
    {
        lua_pushstring(L, "Cannot find the table value.");
        lua_error(L);
    }
    return 1;
}