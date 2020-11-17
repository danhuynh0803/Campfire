#include "Scripting/Lua/LuaEntity.h"
#include "Scene/Component.h"

int LuaEntity::SetEntityPosition(lua_State* L)
{
    Entity* entity = (Entity*)lua_touserdata(L, lua_upvalueindex(1));
    assert(lua_isnumber(L, -3));
    assert(lua_isnumber(L, -2));
    assert(lua_isnumber(L, -1));
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    //transfromComponet->position = glm::vec3(x, y, z);
    return 0;
}

const luaL_Reg LuaEntity::entityTransformLib[] =
{
    { "SetPosition", LuaEntity::SetEntityPosition },
    { NULL, NULL }
};