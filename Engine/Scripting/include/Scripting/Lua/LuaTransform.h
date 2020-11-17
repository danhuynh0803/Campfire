#pragma once

#include <lua.hpp>
#include "Scene/Component.h"

//static const struct luaL_reg {
//    char* methodName;
//    std::function<int(lua_State*)> method; or //lua_CFunction method
//} transformLib[] =
//{
//    {"New", NewTransform},
//    {"SetPosition", SetPosition},
//    {"SetEuler", SetRotation},
//    {"SetScale", SetScale},
//
//    {"GetPosition", GetPosition},
//    {"GetEuler", GetRotation},
//    {"GetScale", GetScale},
//    {NULL, NULL}
//};

static int NewTransform(lua_State* L)
{
    void* transformComponent = lua_newuserdata(L, sizeof(TransformComponent));
    new (transformComponent) TransformComponent();//using the pointer to use the allocated memory that was already alloacted by lua
    luaL_getmetatable(L, "TransComMT");
    assert(lua_istable(L, -1));
    lua_setmetatable(L, -2);

    /*lua_newtable(L);
    lua_setuservalue(L, 1);*/

    return 1;
}

static int GetPosition(lua_State* L)
{
    TransformComponent* tc = (TransformComponent*)lua_touserdata(L, -1);
    lua_pushnumber(L, tc->position.x);
    lua_pushnumber(L, tc->position.y);
    lua_pushnumber(L, tc->position.z);

    return 1;
}

static int GetRotation(lua_State* L)
{
    TransformComponent* tc = (TransformComponent*)lua_touserdata(L, -1);
    lua_pushnumber(L, tc->rotation.x);
    lua_pushnumber(L, tc->rotation.y);
    lua_pushnumber(L, tc->rotation.z);
    return 1;
}

static int GetScale(lua_State* L)
{
    TransformComponent* tc = (TransformComponent*)lua_touserdata(L, -1);
    lua_pushnumber(L, tc->scale.x);
    lua_pushnumber(L, tc->scale.y);
    lua_pushnumber(L, tc->scale.z);
    return 1;
}

static int SetPosition(lua_State* L)
{
    assert(lua_isuserdata(L, -4));
    assert(lua_isnumber(L, -3));
    assert(lua_isnumber(L, -2));
    assert(lua_isnumber(L, -1));
    TransformComponent* tc = (TransformComponent*)lua_touserdata(L, -4);
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    tc->position = glm::vec3(x, y, z);
    return 0;
}

static int SetRotation(lua_State* L)
{
    TransformComponent* tc = (TransformComponent*)lua_touserdata(L, -4);
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    tc->rotation = glm::quat(glm::vec3(
        glm::radians(x),
        glm::radians(y),
        glm::radians(z)));
    return 0;
}

static int SetScale(lua_State* L)
{
    TransformComponent* tc = (TransformComponent*)lua_touserdata(L, -4);
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    tc->scale = glm::vec3(x,y,z);
    return 0;
}

static int ResetTransform(lua_State* L)
{
    TransformComponent* tc = (TransformComponent*)lua_touserdata(L, -1);
    //call tc destructor
    return 0;
}

static int LuaTransformTableIndex(lua_State* L)
{
    assert(lua_isuserdata(L, -2));
    assert(lua_isstring(L, -1));

    TransformComponent* tc = (TransformComponent*)lua_touserdata(L, -2);
    const char* index = lua_tostring(L, -1);
    if (strcmp(index, "x") == 0)
    {
        lua_pushnumber(L, tc->position.x);
        return 1;
    }
    else if (strcmp(index, "y") == 0)
    {
        lua_pushnumber(L, tc->position.y);
        return 1;
    }
    else if (strcmp(index, "z") == 0)
    {
        lua_pushnumber(L, tc->position.z);
        return 1;
    }
    else
    {
        lua_getglobal(L, "Transform");
        lua_pushstring(L, index);
        lua_rawget(L, -2);

        //lua_getuservalue(L, 1);
        //lua_pushvalue(L, 2);
        //lua_gettable(L, -2);
        //if (lua_isnil(L, -1))
        //{
        //    lua_getglobal(L, "Transform");
        //    lua_pushstring(L, index);
        //    lua_rawget(L, -2);
        //}
        return 1;
    }
}

static const luaL_Reg transformLib[] =
{
    {"New", NewTransform},
    {"SetPosition", SetPosition},
    {"SetEuler", SetRotation},
    {"SetScale", SetScale},

    {"GetPosition", GetPosition},
    {"GetEuler", GetRotation},
    {"GetScale", GetScale},
    {NULL, NULL}
};