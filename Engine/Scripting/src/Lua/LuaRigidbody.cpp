#include "Scripting/Lua/LuaRigidbody.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"

int LuaRigidbody::SetVelocity(lua_State* L)
{
    Rigidbody* rbPtr = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    //Entity* entity = (Entity*)lua_getuservalue(L, lua_upvalueindex(1));
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);

    rbPtr->SetVelocity(glm::vec3(x, y, z));
    return 0;
}

int LuaRigidbody::AddVelocity(lua_State* L)
{
    Rigidbody* rb = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    rb->AddVelocity(glm::vec3(x, y, z));

    //Entity* entity = (Entity*)lua_getuservalue(L, lua_upvalueindex(1));
    //if (entity->HasComponent<RigidbodyComponent>())
    //{
    //    entity->GetComponent<RigidbodyComponent>().rigidbody->AddVelocity(glm::vec3(x, y, z));
    //}
    //else
    //{
    //    LOG_ERROR("Entity missing RigidbodyComponent")
    //}
    return 0;
}

int LuaRigidbody::GetVelocity(lua_State* L)
{
    Rigidbody* rb = (Rigidbody*)lua_touserdata(L, lua_upvalueindex(1));
    //Entity* entity = (Entity*)lua_getuservalue(L, lua_upvalueindex(1));
    glm::vec3 vel = rb->GetVelocity();
    lua_pushstring(L, "x");
    lua_pushnumber(L, vel.x);
    lua_settable(L, -3);
    lua_pushstring(L, "y");
    lua_pushnumber(L, vel.y);
    lua_settable(L, -3);
    lua_pushstring(L, "z");
    lua_pushnumber(L, vel.z);
    lua_settable(L, -3);

    /*
    if (entity->HasComponent<RigidbodyComponent>())
    {
        lua_pushstring(L, "x");
        lua_pushnumber(L, entity->GetComponent<RigidbodyComponent>().rigidbody->GetVelocity().getX());
        lua_settable(L, -3);
        lua_pushstring(L, "y");
        lua_pushnumber(L, entity->GetComponent<RigidbodyComponent>().rigidbody->GetVelocity().getX());
        lua_settable(L, -3);
        lua_pushstring(L, "z");
        lua_pushnumber(L, entity->GetComponent<RigidbodyComponent>().rigidbody->GetVelocity().getX());
        lua_settable(L, -3);
    }
    else
    {
        LOG_ERROR("Entity missing RigidbodyComponent")
    }
    */

    return 1;
}

int LuaRigidbody::SetMass(lua_State* L)
{
    Entity* entity = (Entity*)lua_getuservalue(L, lua_upvalueindex(1));
    lua_Number mass = lua_tonumber(L, -1);
    if (entity->HasComponent<RigidbodyComponent>())
    {
        entity->GetComponent<RigidbodyComponent>().rigidbody->mass = mass;
    }
    else
    {
        LOG_ERROR("Entity missing RigidbodyComponent")
    }
    return 0;
}

int LuaRigidbody::GetMass(lua_State* L)
{
    Entity* entity = (Entity*)lua_getuservalue(L, lua_upvalueindex(1));
    if (entity->HasComponent<RigidbodyComponent>())
    {
        lua_pushnumber(L, entity->GetComponent<RigidbodyComponent>().rigidbody->mass);
    }
    else
    {
        LOG_ERROR("Entity missing RigidbodyComponent")
    }
    return 0;
}

int LuaRigidbody::SetDrag(lua_State* L)
{
    Entity* entity = (Entity*)lua_getuservalue(L, lua_upvalueindex(1));
    lua_Number drag = lua_tonumber(L, -1);
    if (entity->HasComponent<RigidbodyComponent>())
    {
        entity->GetComponent<RigidbodyComponent>().rigidbody->drag = drag;
    }
    else
    {
        LOG_ERROR("Entity missing RigidbodyComponent")
    }
    return 0;
}

int LuaRigidbody::GetDrag(lua_State* L)
{
    Entity* entity = (Entity*)lua_getuservalue(L, lua_upvalueindex(1));
    if (entity->HasComponent<RigidbodyComponent>())
    {
        lua_pushnumber(L, entity->GetComponent<RigidbodyComponent>().rigidbody->drag);
    }
    else
    {
        LOG_ERROR("Entity missing RigidbodyComponent")
    }
    return 0;
}

int LuaRigidbody::SetAngularDrag(lua_State* L)
{
    Entity* entity = (Entity*)lua_getuservalue(L, lua_upvalueindex(1));
    lua_Number angularDrag = lua_tonumber(L, -1);
    if (entity->HasComponent<RigidbodyComponent>())
    {
        entity->GetComponent<RigidbodyComponent>().rigidbody->angularDrag = angularDrag;
    }
    else
    {
        LOG_ERROR("Entity missing RigidbodyComponent")
    }
    return 0;
}

int LuaRigidbody::GetAngularDrag(lua_State* L)
{
    Entity* entity = (Entity*)lua_getuservalue(L, lua_upvalueindex(1));
    if (entity->HasComponent<RigidbodyComponent>())
    {
        lua_pushnumber(L, entity->GetComponent<RigidbodyComponent>().rigidbody->angularDrag);
    }
    else
    {
        LOG_ERROR("Entity missing RigidbodyComponent")
    }
    return 0;
}
