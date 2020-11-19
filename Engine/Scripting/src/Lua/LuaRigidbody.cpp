#include "Scripting/Lua/LuaRigidbody.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"

int LuaRigidbody::SetVelocity(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);

    rigidbody->SetVelocity(glm::vec3(x, y, z));
    return 0;
}

int LuaRigidbody::AddVelocity(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    rigidbody->AddVelocity(glm::vec3(x, y, z));

    return 0;
}

int LuaRigidbody::GetVelocity(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    glm::vec3 vel = rigidbody->GetVelocity();
    lua_pushstring(L, "x");
    lua_pushnumber(L, vel.x);
    lua_settable(L, -3);
    lua_pushstring(L, "y");
    lua_pushnumber(L, vel.y);
    lua_settable(L, -3);
    lua_pushstring(L, "z");
    lua_pushnumber(L, vel.z);
    lua_settable(L, -3);

    return 1;
}

int LuaRigidbody::SetMass(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number mass = lua_tonumber(L, -1);
    rigidbody->mass = mass;
    return 0;
}

int LuaRigidbody::GetMass(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_pushnumber(L, rigidbody->mass);
    return 1;
}

int LuaRigidbody::SetDrag(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number drag = lua_tonumber(L, -1);
    rigidbody->drag = drag;
    return 0;
}

int LuaRigidbody::GetDrag(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_pushnumber(L, rigidbody->drag);
    return 0;
}

int LuaRigidbody::SetAngularDrag(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number angularDrag = lua_tonumber(L, -1);
    rigidbody->angularDrag = angularDrag;
    return 1;
}

int LuaRigidbody::GetAngularDrag(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_pushnumber(L, rigidbody->angularDrag);
    return 0;
}
