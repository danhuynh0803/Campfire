#include <string>
#include "Scene/Component.h"
#include "Scripting/Lua/LuaTag.h"

int LuaTag::GetTag(lua_State* L)
{
    const char* tag = (const char*)lua_touserdata(L, lua_upvalueindex(1));
    lua_pushstring(L, tag);
    return 1;
}

const luaL_Reg LuaTag::tagLib[] =
{
    {NULL, NULL}
};