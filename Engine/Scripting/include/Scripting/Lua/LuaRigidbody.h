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
    int SetUseGravity(lua_State*);
    int GetUseGravity(lua_State*);
    int SetFreezePosition(lua_State*);
    int GetFreezePosition(lua_State*);
    int SetFreezeRotation(lua_State*);
    int GetFreezeRotation(lua_State*);
    int SetBodyType(lua_State*);
    int GetBodyType(lua_State*);
}