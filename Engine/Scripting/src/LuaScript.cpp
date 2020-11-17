#include "Scene/ScriptableEntity.h"
#include "Core/Log.h"
#include "Scripting/LuaScript.h"
#include "Scripting/Lua/LuaTransform.h"
#include "Scripting/Lua/LuaTag.h"
#include "Scripting/Lua/LuaEntity.h"

void LuaScript::Start()
{
    L = luaL_newstate();
    luaL_openlibs(L); //opens all standard Lua libraries
    
    lua_newtable(L);
    luaL_setfuncs(L, LuaTransfrom::transformLib, 0);//uses set field on the table
    lua_setglobal(L, "Transform");//names the table Transform

    luaL_newmetatable(L, "TransComMT");
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, LuaTransfrom::LuaTransformTableIndex);//indexing method for the Transfrom table above
    lua_settable(L, -3); //sets the (meta)table and pop above

    lua_newtable(L);
    luaL_setfuncs(L, LuaEntity::entityTransformLib, 0);
    lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().position));
    lua_pushcclosure(L, LuaEntity::SetEntityPosition, 1);
    lua_setfield(L, -2, "SetPosition");
    lua_setglobal(L, "entity");//name the table entity

    lua_newtable(L);
    int luaTagTableIndex = lua_gettop(L);
    luaL_setfuncs(L, LuaTag::tagLib, 0);
    lua_setglobal(L, "Tag");//name the table Tag

    luaL_dofile(L, filepath.c_str());
    lua_getglobal(L, "Start");
    if (lua_pcall(L, 0, 0, 0) != 0)
    {
        LOG_ERROR("Cannot run Start() within {0}", filepath);
        LOG_ERROR("Error {0}", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

void LuaScript::Update(float dt)
{
    luaL_dofile(L, filepath.c_str());
    lua_getglobal(L, "Update");
    lua_pushnumber(L, dt);
    if (lua_pcall(L, 1, 0, 0) != 0)
    {
        LOG_ERROR("Cannot run Update() within {0}", filepath);
        LOG_ERROR("Error {0}", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

void LuaScript::Destroy()
{
    lua_close(L);
}