#pragma once

#include <lua.hpp>
#include <string>

class LuaTag
{
    std::string tag;
};

static const struct luaL_reg tagLib[] =
{
    {"GetTag", GetTag},
    {"SetTag", SetTag},
};

static int GetTag(lua_State* L)
{
    char* tag = lua_touserdata(L, 1);
    luaL_argcheck(L, tag != nullptr, 1, "string expected");
    lua_push
}

static int SetTag(lua_State* L)
{
    char* a = (char*)lua_touserdata(L, 1);
    luaL_argcheck(L, a != NULL, 1, "'string' expected");

    return 0;
}
