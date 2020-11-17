#pragma once

#include <lua.hpp>
#include "Scene/ScriptableEntity.h"
#include "Core/Log.h"

class LuaScript : public ScriptableEntity
{
public:
    void Start()
    {
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);
        luaL_dofile(L, filepath.c_str());
        lua_getglobal(L, "Start");
        if (lua_pcall(L, 0, 0, 0) != 0)
        {
            LOG_ERROR("Cannot run Start() within {0}", filepath);
        }
    }

    void Update(float dt)
    {
        lua_State* L = luaL_newstate();
        luaL_openlibs(L);
        luaL_dofile(L, filepath.c_str());
        lua_getglobal(L, "Update");
        lua_pushnumber(L, dt);
        if (lua_pcall(L, 1, 0, 0) != 0)
        {
            LOG_ERROR("Cannot run Update() within {0}", filepath);
            LOG_ERROR("Error {0}", lua_tostring(L, -1));
        }
    }

};
