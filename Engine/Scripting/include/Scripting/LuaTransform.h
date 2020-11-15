#pragma once

#include <lua.hpp>
#include <functional>

struct luaL_reg {
    char* methodName;
    std::function<int(lua_State*)> method;
} transformLib[] =
{
    {"New", NewTransform},
    {"SetPosition", SetPosition},
    {"SetEuler", SetRotation},
    {"SetScale", SetScale},

    {"GetPosition", GetPosition},
    {"GetEuler", GetRotation},
    {"GetScale", GetScale}
};

static int NewTransform(lua_State* L)
{
}

static int SetPosition(lua_State* L)
{
}

static int SetRotation(lua_State* L)
{
}

static int SetScale(lua_State* L)
{
}

static int GetPosition(lua_State* L)
{
}

static int GetRotation(lua_State* L)
{
}

static int GetScale(lua_State* L)
{
}
