#pragma once

#include <lua.hpp>
#include <string>
#include "Scene/Component.h"

//static const struct luaL_reg tagLib[] =
//{
//    {"GetTag", GetTag},
//    {"SetTag", SetTag},
//};

static int GetTag(lua_State* L)
{
    TagComponent* tagComponent = (TagComponent*)lua_touserdata(L, 1);
    luaL_argcheck(L, tagComponent != nullptr, 1, "string expected");
    lua_pushstring(L, tagComponent->tag.c_str());
    return 1;
}

static int SetTag(lua_State* L)
{
    TagComponent* tagComponent = (TagComponent*)lua_touserdata(L, -2);
    luaL_argcheck(L, tagComponent != NULL, 1, "'string' expected");
    tagComponent->tag = lua_tostring(L, -1);
    return 1;
}
