#include "Scripting/Lua/LuaRigidbody.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"

int LuaRigidbody::SetVelocity(lua_State* L)
{
    int topIndex = lua_gettop(L);
    lua_Number x = luaL_checknumber(L, topIndex - 2);
    lua_Number y = luaL_checknumber(L, topIndex - 1);
    lua_Number z = luaL_checknumber(L, topIndex);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if(!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Velocity info. Possibly was destroyed?");
        lua_error(L);
    }
    rigidbody->SetVelocity(glm::vec3(x, y, z));

    return 0;
}

int LuaRigidbody::AddVelocity(lua_State* L)
{
    int topIndex = lua_gettop(L);
    lua_Number x = luaL_checknumber(L, topIndex - 2);
    lua_Number y = luaL_checknumber(L, topIndex - 1);
    lua_Number z = luaL_checknumber(L, topIndex);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Velocity info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        rigidbody->AddVelocity(glm::vec3(x, y, z));
    }
    return 0;
}

int LuaRigidbody::GetVelocity(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Transfrom Rigidbody/Velocity info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        glm::vec3 vel = rigidbody->GetVelocity();
        lua_newtable(L);
        lua_pushstring(L, "x");
        lua_pushnumber(L, vel.x);
        lua_settable(L, -3);
        lua_pushstring(L, "y");
        lua_pushnumber(L, vel.y);
        lua_settable(L, -3);
        lua_pushstring(L, "z");
        lua_pushnumber(L, vel.z);
        lua_settable(L, -3);
    }
    return 1;
}

int LuaRigidbody::SetMass(lua_State* L)
{
    int topIndex = lua_gettop(L);
    lua_Number mass = luaL_checknumber(L, topIndex);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Transfrom Rigidbody/Mass info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        rigidbody->mass = mass;
    }
    return 0;
}

int LuaRigidbody::GetMass(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Mass info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        lua_pushnumber(L, rigidbody->mass);
    }
    return 1;
}

int LuaRigidbody::SetDrag(lua_State* L)
{
    int topIndex = lua_gettop(L); 
    lua_Number drag = luaL_checknumber(L, topIndex);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Drag info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        rigidbody->drag = drag;
    }
    return 0;
}

int LuaRigidbody::GetDrag(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Drag info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        lua_pushnumber(L, rigidbody->drag);
    }
    return 1;
}

int LuaRigidbody::SetAngularDrag(lua_State* L)
{
    int topIndex = lua_gettop(L);
    lua_Number angularDrag = luaL_checknumber(L, topIndex);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/AngularDrag info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        rigidbody->angularDrag = angularDrag;
    }
    return 0;
}

int LuaRigidbody::GetAngularDrag(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/AngularDrag info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        lua_pushnumber(L, rigidbody->angularDrag);
    }
    return 1;
}

int LuaRigidbody::SetUseGravity(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checktype(L, topIndex, LUA_TBOOLEAN);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Use Gravity info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        rigidbody->useGravity = lua_toboolean(L, -1);
    }
    return 0;
}

int LuaRigidbody::GetUseGravity(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Use Gravity info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        lua_pushboolean(L, rigidbody->useGravity);
    }
    return 1;
}

int LuaRigidbody::SetFreezePosition(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checktype(L, topIndex - 2, LUA_TBOOLEAN);
    luaL_checktype(L, topIndex - 1, LUA_TBOOLEAN);
    luaL_checktype(L, topIndex, LUA_TBOOLEAN);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Freeze Position info. Possibly destroyed?");
        lua_error(L);
    }
    else
    {
        bool x = lua_toboolean(L, -3);
        bool y = lua_toboolean(L, -2);
        bool z = lua_toboolean(L, -1);
        rigidbody->freezePosition[0] = x;
        rigidbody->freezePosition[1] = y;
        rigidbody->freezePosition[2] = z;
    }
    return 0;
}

int LuaRigidbody::GetFreezePosition(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checktype(L, topIndex - 2, LUA_TBOOLEAN);
    luaL_checktype(L, topIndex - 1, LUA_TBOOLEAN);
    luaL_checktype(L, topIndex, LUA_TBOOLEAN);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Freeze Position info. Possibly destroyed?");
        lua_error(L);
    }
    else
    {
        lua_newtable(L);
        lua_pushstring(L, "x");
        lua_pushnumber(L, rigidbody->freezePosition[0]);
        lua_rawset(L, -3);
        lua_pushstring(L, "y");
        lua_pushnumber(L, rigidbody->freezePosition[1]);
        lua_rawset(L, -3);
        lua_pushstring(L, "z");
        lua_pushnumber(L, rigidbody->freezePosition[2]);
        lua_rawset(L, -3);
    }
    return 1;
}

int LuaRigidbody::SetFreezeRotation(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checktype(L, topIndex - 2, LUA_TBOOLEAN);
    luaL_checktype(L, topIndex - 1, LUA_TBOOLEAN);
    luaL_checktype(L, topIndex, LUA_TBOOLEAN);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Freeze Rotation info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        bool x = lua_toboolean(L, -3);
        bool y = lua_toboolean(L, -2);
        bool z = lua_toboolean(L, -1);
        rigidbody->freezeRotation[0] = x;
        rigidbody->freezeRotation[1] = y;
        rigidbody->freezeRotation[2] = z;
    }
    return 0;
}

int LuaRigidbody::GetFreezeRotation(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Freeze Rotation info. Possibly destroyed?");
        lua_error(L);
    }
    else
    {
        lua_newtable(L);
        lua_pushstring(L, "x");
        lua_pushnumber(L, rigidbody->freezeRotation[0]);
        lua_rawset(L, -3);
        lua_pushstring(L, "y");
        lua_pushnumber(L, rigidbody->freezeRotation[1]);
        lua_rawset(L, -3);
        lua_pushstring(L, "z");
        lua_pushnumber(L, rigidbody->freezeRotation[2]);
        lua_rawset(L, -3);
    }
    return 1;
}

int LuaRigidbody::SetBodyType(lua_State* L)
{
    int topIndex = lua_gettop(L);
    //TODO Add Integer/Enum conversion bound check
    const char* type = luaL_checkstring(L, topIndex);
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Set BodyType info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        rigidbody->SetBodyType(std::string(type));
    }
    return 0;
}

int LuaRigidbody::GetBodyType(lua_State* L)
{
    Rigidbody* rigidbody = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    if (!rigidbody)
    {
        lua_pushstring(L, "Missing Rigidbody/Set BodyType info. Possibly was destroyed?");
        lua_error(L);
    }
    else
    {
        lua_pushstring(L, rigidbody->GetBodyType().c_str());
    }
    return 1;
}