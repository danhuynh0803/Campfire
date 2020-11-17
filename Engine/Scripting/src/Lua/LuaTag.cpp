#include <string>
#include "Scene/Component.h"
#include "Scripting/Lua/LuaTag.h"

int LuaTag::GetTag(lua_State* L)
{
    TagComponent* tagComponent = (TagComponent*)lua_touserdata(L, -1);
    luaL_argcheck(L, tagComponent != nullptr, 1, "string expected");
    const char* tag = tagComponent->tag.c_str();
    lua_pushstring(L, tag);
    return 1;
}

int LuaTag::SetTag(lua_State* L)
{
    TagComponent* tagComponent = (TagComponent*)lua_touserdata(L, -2);
    luaL_argcheck(L, tagComponent != NULL, 1, "'string' expected");
    const char* tag = lua_tostring(L, 2);
    tagComponent->tag = tag;
    return 0;
}

int LuaTag::PushAOne(lua_State* L)
{
    lua_pushnumber(L, 1);
    return 1;
}

const luaL_Reg LuaTag::tagLib[] =
{
    {"GetTag", LuaTag::GetTag},
    {"SetTag", LuaTag::SetTag},
    {"PushAOne",LuaTag::PushAOne},
    {NULL, NULL}
};