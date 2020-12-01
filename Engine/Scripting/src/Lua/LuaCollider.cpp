#include "Scripting/Lua/LuaCollider.h"
#include <glm/glm.hpp>

int LuaCollider::Box::GetCenter(lua_State* L)
{
    glm::vec3* center = (glm::vec3*)lua_touserdata(L, lua_upvalueindex(1));
    lua_newtable(L);
    lua_pushstring(L, "x");
    lua_pushnumber(L, center->x);
    lua_settable(L, -3);
    lua_pushstring(L, "y");
    lua_pushnumber(L, center->y);
    lua_settable(L, -3);
    lua_pushstring(L, "z");
    lua_pushnumber(L, center->z);
    lua_settable(L, -3);

    return 1;
}

int LuaCollider::Sphere::GetCenter(lua_State* L)
{
    glm::vec3* center = (glm::vec3*)lua_touserdata(L, lua_upvalueindex(1));
    lua_newtable(L);
    lua_pushstring(L, "x");
    lua_pushnumber(L, center->x);
    lua_settable(L, -3);
    lua_pushstring(L, "y");
    lua_pushnumber(L, center->y);
    lua_settable(L, -3);
    lua_pushstring(L, "z");
    lua_pushnumber(L, center->z);
    lua_settable(L, -3);

    return 1;
}

int LuaCollider::Capsule::GetCenter(lua_State* L)
{
    glm::vec3* center = (glm::vec3*)lua_touserdata(L, lua_upvalueindex(1));
    lua_newtable(L);
    lua_pushstring(L, "x");
    lua_pushnumber(L, center->x);
    lua_settable(L, -3);
    lua_pushstring(L, "y");
    lua_pushnumber(L, center->y);
    lua_settable(L, -3);
    lua_pushstring(L, "z");
    lua_pushnumber(L, center->z);
    lua_settable(L, -3);

    return 1;
}