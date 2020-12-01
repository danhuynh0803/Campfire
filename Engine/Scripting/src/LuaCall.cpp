#include "Scripting/LuaCall.h"
#include "Scripting/LuaScript.h"
#include <string.h>
#include "Core/Log.h"

int ContinueFunction(lua_State* L, int status, lua_KContext ctx)
{
    switch (status)
    {

    }
    return 0;
}

int LuaCall::Start(lua_State* L)
{
    const char* filePath = lua_tostring(L, lua_upvalueindex(1));
    int i = luaL_dofile(L, filePath);
    lua_pushcfunction(L, LuaCallback);
    lua_getglobal(L, "Start");
    //int status = lua_pcallk(L, 0, 0, -2, 0, ContinueFunction);
    int status = lua_pcall(L, 0, 0, -2);
    switch (status)
    {
        case LUA_OK:
            break;
        case LUA_ERRERR:
            LOG_ERROR("Cannot run Start() within {0}. Error: {1}", filePath, lua_tostring(L, -1));
            lua_pop(L, 1);
            break;
        case LUA_YIELD:
            break;
        case LUA_ERRRUN:
            if (lua_tostring(L, -1) == "Destroy Entity")
            {
                LuaScript* script = (LuaScript*)lua_touserdata(L, lua_upvalueindex(2));
                script->Destroy();
            }
            break;
        default:
            break;
    }
    lua_pop(L, 1);
    return 0;
}

int LuaCall::Update(lua_State* L)
{
    const char* filePath = lua_tostring(L, lua_upvalueindex(1));
    luaL_dofile(L, filePath);
    lua_pushcfunction(L, LuaCallback);
    lua_getglobal(L, "Update");
    int status = lua_pcall(L, 0, 0, -2);
    switch (status)
    {
        case LUA_OK:
            break;
        case LUA_ERRERR:
            LOG_ERROR("Cannot run Update() within {0}. Error: {1}", filePath, lua_tostring(L, -1));
            lua_pop(L, 1);
            break;
        case LUA_YIELD:
            break;
        case LUA_ERRRUN:
            if (lua_tostring(L, -1) == "Destroy Entity")
            {
                LuaScript* script = (LuaScript*)lua_touserdata(L, lua_upvalueindex(2));
                script->Destroy();
            }
            break;
        default:
            break;
    }
    lua_pop(L, 1);
    return 0;
}

int LuaCall::OnTriggerEnter(lua_State* L)
{
    const char* filePath = lua_tostring(L, lua_upvalueindex(1));
    luaL_dofile(L, filePath);
    lua_pushcfunction(L, LuaCallback);
    lua_getglobal(L, "OnTriggerEnter");
    LuaScript* script = (LuaScript*)lua_touserdata(L, lua_upvalueindex(2));
    Entity* entity = (Entity*)lua_touserdata(L, lua_upvalueindex(3));
    script->LuaPushEntity(*entity, L);
    int status = lua_pcall(L, 1, 0, -3);
    switch (status)
    {
        case LUA_OK:
            break;
        case LUA_ERRERR:
            LOG_ERROR("Cannot run OnTriggerEnter() within {0}. Error: {1}", filePath, lua_tostring(L, -1));
            lua_pop(L, 1);
            break;
        case LUA_YIELD:
            break;
        default:
            break;
    }
    lua_pop(L, 1);
    return 0;
}

int LuaCall::OnTriggerStay(lua_State* L)
{
    const char* filePath = lua_tostring(L, lua_upvalueindex(1));
    luaL_dofile(L, filePath);
    lua_pushcfunction(L, LuaCallback);
    lua_getglobal(L, "OnTriggerStay");
    LuaScript* script = (LuaScript*)lua_touserdata(L, lua_upvalueindex(2));
    Entity* entity = (Entity*)lua_touserdata(L, lua_upvalueindex(3));
    script->LuaPushEntity(*entity, L);
    int status = lua_pcallk(L, 1, 0, -3, 0, ContinueFunction);
    switch (status)
    {
        case LUA_OK:
            break;
        case LUA_ERRERR:
            LOG_ERROR("Cannot run OnTriggerStay() within {0}. Error: {1}", filePath, lua_tostring(L, -1));
            lua_pop(L, 1);
            break;
        case LUA_YIELD:
            break;
        default:
            break;
    }
    lua_pop(L, 1);
    return 0;
}

int LuaCall::OnTriggerExit(lua_State* L)
{
    const char* filePath = lua_tostring(L, lua_upvalueindex(1));
    luaL_dofile(L, filePath);
    lua_pushcfunction(L, LuaCallback);
    lua_getglobal(L, "OnTriggerExit");
    if (!lua_istable(L, -1)) return 0;
    LuaScript* script = (LuaScript*)lua_touserdata(L, lua_upvalueindex(2));
    Entity* entity = (Entity*)lua_touserdata(L, lua_upvalueindex(3));
    script->LuaPushEntity(*entity, L);
    int status = lua_pcallk(L, 1, 0, -3, 0, ContinueFunction);
    switch (status)
    {
        case LUA_OK:
            break;
        case LUA_ERRERR:
            LOG_ERROR("Cannot run OnTriggerExit within {0}. Error: {1}", filePath, lua_tostring(L, -1));
            lua_pop(L, 1);
            break;
        case LUA_YIELD:
            break;
        default:
            break;
    }
    lua_pop(L, 1);
    return 0;
}
