#include "Scripting/Lua/LuaGlobal.h"
#include "Scripting/LuaManager.h"
#include "Core/Log.h"

int LuaGlobal::SetNumber(lua_State* L)
{
    int topIndex = lua_gettop(L);
    lua_Number number = luaL_checknumber(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex - 1);
    LuaManager::SetGlobalLuaTableNumber(LUA_MANAGER_GLOBAL, name, number);
    return 0;
}

int LuaGlobal::SetBoolean(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checktype(L, topIndex, LUA_TBOOLEAN);
    bool value = lua_toboolean(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex - 1);
    LuaManager::SetGlobalLuaTableBoolean(LUA_MANAGER_GLOBAL, name, value);
    return 0;
}

int LuaGlobal::SetInteger(lua_State* L)
{
    int topIndex = lua_gettop(L);
    lua_Integer nunber = luaL_checkinteger(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex - 1);
    LuaManager::SetGlobalLuaTableInteger(LUA_MANAGER_GLOBAL, name, nunber);
    return 0;
}

int LuaGlobal::SetString(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* value = luaL_checkstring(L, topIndex);
    const char* name = luaL_checkstring(L, topIndex - 1);
    LuaManager::SetGlobalLuaTableString(LUA_MANAGER_GLOBAL, name, value);
    return 0;
}

int LuaGlobal::SetTable(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checktype(L, topIndex, LUA_TTABLE);
    const char* name = luaL_checkstring(L, topIndex - 1);
    if (!LuaManager::SetGlobalLuaTableTable(L, LUA_MANAGER_GLOBAL, name))
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
    if (!LuaManager::GetGlobalLuaTableNumber(L, LUA_MANAGER_GLOBAL, name))
    {
        lua_pushstring(L, "Cannot find the double value.");
        lua_error(L);
    }
    return 1;
}

int LuaGlobal::GetBoolean(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    if (!LuaManager::GetGlobalLuaTableBoolean(L, LUA_MANAGER_GLOBAL, name))
    {
        lua_pushstring(L, "Cannot find the boolean value.");
        lua_error(L);
    }
    return 1;
}

int LuaGlobal::GetInteger(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    if (!LuaManager::GetGlobalLuaTableInteger(L, LUA_MANAGER_GLOBAL, name))
    {
        lua_pushstring(L, "Cannot find the integer value.");
        lua_error(L);
    }
    return 1;
}

int LuaGlobal::GetString(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    if (!LuaManager::GetGlobalLuaTableString(L, LUA_MANAGER_GLOBAL, name))
    {
        lua_pushstring(L, "Cannot find the string value.");
        lua_error(L);
    }
    return 1;
}

int LuaGlobal::GetTable(lua_State* L)
{
    int topIndex = lua_gettop(L);
    const char* name = luaL_checkstring(L, topIndex);
    if (!LuaManager::GetGlobalLuaTableTable(L, LUA_MANAGER_GLOBAL, name))
    {
        lua_pushstring(L, "Cannot find the table value.");
        lua_error(L);
    }
    return 1;
}