#pragma once

#include <lua.hpp>

static int SetVector3(lua_State* L)
{
    glm::vec3* v = (glm::vec3*)lua_touserdata(L, 1);
}
