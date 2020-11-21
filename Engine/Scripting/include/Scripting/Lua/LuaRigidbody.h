#pragma once
#include <lua.hpp>

namespace LuaRigidbody
{
    int SetVelocity(lua_State*);
    int AddVelocity(lua_State*);
    int GetVelocity(lua_State*);
    int SetMass(lua_State*);
    int GetMass(lua_State*);
    int SetDrag(lua_State*);
    int GetDrag(lua_State*);
    int SetAngularDrag(lua_State*);
    int GetAngularDrag(lua_State*);
    int UseGravity(lua_State*);
    int FreezePosition(lua_State*);
    int FreezeRotation(lua_State*);
}
