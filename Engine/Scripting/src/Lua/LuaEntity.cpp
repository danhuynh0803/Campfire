#include "Scripting/Lua/LuaEntity.h"

int LuaEntity::Instantiate(lua_State*)
{
    return 0;
}

const luaL_Reg LuaEntity::entityLib[] =
{
    { NULL, NULL }
};
