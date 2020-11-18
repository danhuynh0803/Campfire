#include "Scene/ScriptableEntity.h"
#include "Core/Log.h"
#include "Scripting/LuaScript.h"
#include "Scripting/Lua/LuaTransform.h"
#include "Scripting/Lua/LuaTag.h"
#include "Scripting/Lua/LuaEntity.h"
#include "Scripting/Lua/LuaInput.h"

void LuaScript::Start()
{
    L = luaL_newstate();
    luaL_openlibs(L); //opens all standard Lua libraries

    char* transformComponetMetaTableName = "TransComMT";

    lua_newtable(L);
    {
        luaL_setfuncs(L, LuaTransfrom::transformLib, 0);
        lua_pushstring(L, transformComponetMetaTableName);
        lua_pushcclosure(L, LuaTransfrom::NewTransform, 1);
        lua_setfield(L, -2, "New");
    }lua_setglobal(L, "Transform");
    
    luaL_newmetatable(L, transformComponetMetaTableName);
    {
        lua_pushstring(L, "__index");
        lua_pushcfunction(L, LuaTransfrom::LuaTransformTableIndex);//indexing method for the Transfrom table above
    }lua_settable(L, -3); //sets the (meta)table and pop above
    
    lua_newtable(L);
    {
        //luaL_setfuncs(L, LuaTransfrom::transformLib, 0);
        
        lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().position));
        lua_pushcclosure(L, LuaTransfrom::SetEntityPosition, 1);
        lua_setfield(L, -2, "SetPosition");

        lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().euler));
        lua_pushcclosure(L, LuaTransfrom::SetEntityRotation, 1);
        lua_setfield(L, -2, "SetRotation");

        lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().scale));
        lua_pushcclosure(L, LuaTransfrom::SetEntityScale, 1);
        lua_setfield(L, -2, "SetScale");

        lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().position));
        lua_pushcclosure(L, LuaTransfrom::Translate, 1);
        lua_setfield(L, -2, "Translate");
    }lua_setglobal(L, "Transform");

    lua_newtable(L);
    {
        //lua_getglobal(L, "Transform");
        //lua_setfield(L, -1, "Transform");
    }lua_setglobal(L, "entity"); //name the table entity

    lua_newtable(L);
    {
        luaL_setfuncs(L, LuaTag::tagLib, 0);
    }lua_setglobal(L, "Tag");//name the table Tag

    lua_newtable(L);
    {
        luaL_setfuncs(L, LuaInput::inputLib, 0);
    }lua_setglobal(L, "Input");//name the table Input

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
