#pragma once

#include <lua.hpp>
#include "Scene/ScriptableEntity.h"
#include "Core/Log.h"
#include "Scripting/Lua/LuaTransform.h"
#include "Scripting/Lua/LuaTag.h"
#include "Scripting/Lua/LuaEntity.h"

class LuaScript : public ScriptableEntity
{
public:

    void Start()
    {
        L = luaL_newstate();
        luaL_openlibs(L);
        lua_newtable(L);
        luaL_setfuncs(L, LuaTransfrom::transformLib, 0);
        lua_setglobal(L, "Transform");

        luaL_newmetatable(L, "TransComMT");
        lua_pushstring(L, "__index");
        lua_pushcfunction(L, LuaTransfrom::LuaTransformTableIndex);
        lua_settable(L, -3);

        lua_newtable(L);
        luaL_setfuncs(L, LuaEntity::entityTransformLib, 0);
        lua_setglobal(L, "entity");

        constexpr int NUMBER_OF_UPVALUES = 1;
        //lua_pushlightuserdata(L, &entity);s
        //lua_pushcclosure(L, LuaEntity::SetEntityPosition, NUMBER_OF_UPVALUES);
        lua_pushlightuserdata(L, &(GetComponent<TransformComponent>().position));
        lua_pushcclosure(L, LuaEntity::SetEntityPosition, NUMBER_OF_UPVALUES);

        lua_newtable(L);
        int luaTagTableIndex = lua_gettop(L);
        luaL_setfuncs(L, LuaTag::tagLib, 0);
        lua_setglobal(L, "Tag");

        luaL_dofile(L, filepath.c_str());
        lua_getglobal(L, "Start");
        if (lua_pcall(L, 0, 0, 0) != 0)
        {
            LOG_ERROR("Cannot run Start() within {0}", filepath);
        }
    }

    void Update(float dt)
    {
        luaL_dofile(L, filepath.c_str());
        lua_getglobal(L, "Update");
        lua_pushnumber(L, dt);
        if (lua_pcall(L, 1, 0, 0) != 0)
        {
            LOG_ERROR("Cannot run Update() within {0}", filepath);
            LOG_ERROR("Error {0}", lua_tostring(L, -1));
        }
    }

private:
    lua_State* L;
};
