#include "Scripting/Lua/LuaEntity.h"
#include "Scene/Component.h"

int LuaEntity::SetEntityPosition(lua_State* L)
{
    glm::vec3* position = (glm::vec3*)lua_touserdata(L,lua_upvalueindex(1));
    assert(position);
    assert(lua_isnumber(L, -3));
    assert(lua_isnumber(L, -2));
    assert(lua_isnumber(L, -1));
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    position->x = x;
    position->y = y;
    position->z = z;
    return 0;
}

const luaL_Reg LuaEntity::entityTransformLib[] =
{
    { NULL, NULL }
};