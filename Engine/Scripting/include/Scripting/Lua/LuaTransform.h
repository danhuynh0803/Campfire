#pragma once

#include <lua.hpp>

static const struct luaL_reg transformLib[] =
{
    {"New", NewTransform},
    {"SetPosition", SetPosition},
    {"SetEuler", SetEuler},
    {"SetScale", SetScale},

    {"GetPosition", GetPosition},
    {"GetEuler", GetEuler},
    {"GetScale", GetScale},
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

static int GetEuler(lua_State* L)
{
}

static int GetScale(lua_State* L)
{
}
