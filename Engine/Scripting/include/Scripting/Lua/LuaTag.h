#pragma once

#include <lua.hpp>
#include <string>
#include "Scene/Component.h"

static int GetTag(lua_State* L)
{
    TagComponent* tagComponent = (TagComponent*)lua_touserdata(L, -1);
    luaL_argcheck(L, tagComponent != nullptr, 1, "string expected");
    const char* tag = tagComponent->tag.c_str();
    lua_pushstring(L, tag);
    return 1;
}

static int SetTag(lua_State* L)
{
    TagComponent* tagComponent = (TagComponent*)lua_touserdata(L, -2);
    luaL_argcheck(L, tagComponent != NULL, 1, "'string' expected");
    const char* tag = lua_tostring(L, 2);
    tagComponent->tag = tag;
    return 1;
}

static const luaL_Reg tagLib[] =
{
    {"GetTag", GetTag},
    {"SetTag", SetTag},
    {NULL, NULL}
};
