#include "Scripting/Lua/LuaRigidbody.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"

int LuaRigidbody::SetVelocity(lua_State* L)
{
    luaL_checknumber(L, 1);
    luaL_checknumber(L, 2);
    luaL_checknumber(L, 3);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);

    rigidbody->SetVelocity(glm::vec3(x, y, z));
    return 0;
}

int LuaRigidbody::AddVelocity(lua_State* L)
{
    luaL_checknumber(L, 1);
    luaL_checknumber(L, 2);
    luaL_checknumber(L, 3);
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
    luaL_checknumber(L, 1);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number mass = lua_tonumber(L, -1);
    rigidbody->mass = mass;
    return 0;
}

int LuaRigidbody::GetMass(lua_State* L)
{
    luaL_checknumber(L, 1);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_pushnumber(L, rigidbody->mass);
    return 1;
}

int LuaRigidbody::SetDrag(lua_State* L)
{
    luaL_checknumber(L, 1);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number drag = lua_tonumber(L, -1);
    rigidbody->drag = drag;
    return 0;
}

int LuaRigidbody::GetDrag(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_pushnumber(L, rigidbody->drag);
    return 1;
}

int LuaRigidbody::SetAngularDrag(lua_State* L)
{
    luaL_checknumber(L, 1);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number angularDrag = lua_tonumber(L, -1);
    rigidbody->angularDrag = angularDrag;
    return 0;
}

int LuaRigidbody::GetAngularDrag(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_pushnumber(L, rigidbody->angularDrag);
    return 1;
}

int LuaRigidbody::UseGravity(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    rigidbody->useGravity = lua_toboolean(L, -1);
    return 0;
}

int LuaRigidbody::FreezePosition(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    luaL_checktype(L, 3, LUA_TBOOLEAN);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    bool x = lua_toboolean(L, -3);
    bool y = lua_toboolean(L, -2);
    bool z = lua_toboolean(L, -1);
    rigidbody->freezePosition[0] = x;
    rigidbody->freezePosition[1] = y;
    rigidbody->freezePosition[2] = z;
    return 0;
}

int LuaRigidbody::FreezeRotation(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    luaL_checktype(L, 3, LUA_TBOOLEAN);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    bool x = lua_toboolean(L, -3);
    bool y = lua_toboolean(L, -2);
    bool z = lua_toboolean(L, -1);
    rigidbody->freezeRotation[0] = x;
    rigidbody->freezeRotation[1] = y;
    rigidbody->freezeRotation[2] = z;
    return 0;
}