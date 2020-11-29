#include "Scripting/Lua/LuaTransform.h"
#include "Scene/Component.h"

//int LuaTransfrom::NewTransform(lua_State* L)
//{
//    const char* transformCompoentMetaTableName = lua_tostring(L, lua_upvalueindex(1));
//    void* transformComponent = lua_newuserdata(L, sizeof(TransformComponent));
//    new (transformComponent) TransformComponent();//using the pointer to use the allocated memory that was already alloacted by lua
//    luaL_getmetatable(L, transformCompoentMetaTableName);
//    lua_setmetatable(L, -2);
//
//    /*lua_newtable(L);
//    lua_setuservalue(L, 1);*/
//
//    return 1;
//}
//
//int LuaTransfrom::GetPosition(lua_State* L)
//{
//    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -1);
//    lua_pushstring(L, "x");
//    lua_pushnumber(L, transfromComponet->position.x);
//    lua_settable(L, -3);
//    lua_pushstring(L, "y");
//    lua_pushnumber(L, transfromComponet->position.y);
//    lua_settable(L, -3);
//    lua_pushstring(L, "z");
//    lua_pushnumber(L, transfromComponet->position.z);
//    lua_settable(L, -3);
//
//    return 1;
//}
//
//int LuaTransfrom::GetRotation(lua_State* L)
//{
//    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -1);
//    lua_pushstring(L, "x");
//    lua_pushnumber(L, transfromComponet->euler.x);
//    lua_settable(L, -3);
//    lua_pushstring(L, "y");
//    lua_pushnumber(L, transfromComponet->euler.y);
//    lua_settable(L, -3);
//    lua_pushstring(L, "z");
//    lua_pushnumber(L, transfromComponet->euler.z);
//    lua_settable(L, -3);
//    return 1;
//}
//
//int LuaTransfrom::GetScale(lua_State* L)
//{
//    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -1);
//    lua_pushstring(L, "x");
//    lua_pushnumber(L, transfromComponet->scale.x);
//    lua_settable(L, -3);
//    lua_pushstring(L, "y");
//    lua_pushnumber(L, transfromComponet->scale.y);
//    lua_settable(L, -3);
//    lua_pushstring(L, "z");
//    lua_pushnumber(L, transfromComponet->scale.z);
//    lua_settable(L, -3);
//    return 1;
//}
//
//int LuaTransfrom::SetPosition(lua_State* L)
//{
//    luaL_checktype(L, 1, LUA_TUSERDATA);
//    luaL_checknumber(L, 2);
//    luaL_checknumber(L, 3);
//    luaL_checknumber(L, 4);
//    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -4);
//    lua_Number x = lua_tonumber(L, -3);
//    lua_Number y = lua_tonumber(L, -2);
//    lua_Number z = lua_tonumber(L, -1);
//    transfromComponet->position = glm::vec3(x, y, z);
//    return 0;
//}
//
//int LuaTransfrom::SetRotation(lua_State* L)
//{
//    luaL_checktype(L, 1, LUA_TUSERDATA);
//    luaL_checknumber(L, 2);
//    luaL_checknumber(L, 3);
//    luaL_checknumber(L, 4);
//    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -4);
//    lua_Number x = lua_tonumber(L, -3);
//    lua_Number y = lua_tonumber(L, -2);
//    lua_Number z = lua_tonumber(L, -1);
//    transfromComponet->euler = glm::vec3(x, y, z);
//    return 0;
//}
//
//int LuaTransfrom::SetScale(lua_State* L)
//{
//    luaL_checktype(L, 1, LUA_TUSERDATA);
//    luaL_checknumber(L, 2);
//    luaL_checknumber(L, 3);
//    luaL_checknumber(L, 4);
//    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -4);
//    lua_Number x = lua_tonumber(L, -3);
//    lua_Number y = lua_tonumber(L, -2);
//    lua_Number z = lua_tonumber(L, -1);
//    transfromComponet->scale = glm::vec3(x, y, z);
//    return 0;
//}
//
////int LuaTransfrom::ResetTransform(lua_State* L)
////{
////    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -1);
////    //call tc destructor
////    return 0;
////}
//
//int LuaTransfrom::LuaTransformTableIndex(lua_State* L)
//{
//    luaL_checktype(L, 1, LUA_TUSERDATA);
//    luaL_checkstring(L, 2);
//
//    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -2);
//    const char* index = lua_tostring(L, -1);
//
//    if (strcmp(index, "x") == 0)
//    {
//        lua_pushnumber(L, transfromComponet->position.x);
//        return 1;
//    }
//    else if (strcmp(index, "y") == 0)
//    {
//        lua_pushnumber(L, transfromComponet->position.y);
//        return 1;
//    }
//    else if (strcmp(index, "z") == 0)
//    {
//        lua_pushnumber(L, transfromComponet->position.z);
//        return 1;
//    }
//    else
//    {
//        lua_getglobal(L, "Transform");
//        lua_pushstring(L, index);
//        lua_rawget(L, -2);
//
//        //lua_getuservalue(L, 1);
//        //lua_pushvalue(L, 2);
//        //lua_gettable(L, -2);
//        //if (lua_isnil(L, -1))
//        //{
//        //    lua_getglobal(L, "Transform");
//        //    lua_pushstring(L, index);
//        //    lua_rawget(L, -2);
//        //}
//        return 1;
//    }
//}
//
//const luaL_Reg LuaTransfrom::transformLib[] =
//{
//    { "SetPosition", LuaTransfrom::SetPosition },
//    { "SetEuler", LuaTransfrom::SetRotation },
//    { "SetScale", LuaTransfrom::SetScale },
//
//    { "GetPosition", LuaTransfrom::GetPosition },
//    { "GetEuler", LuaTransfrom::GetRotation },
//    { "GetScale", LuaTransfrom::GetScale },
//    { NULL, NULL }
//};

int LuaTransfrom::SetEntityPosition(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checknumber(L, topIndex - 2);
    luaL_checknumber(L, topIndex - 1);
    luaL_checknumber(L, topIndex);
    glm::vec3* position = (glm::vec3*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    *position = glm::vec3(x, y, z);
    return 0;
}

int LuaTransfrom::SetEntityRotation(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checknumber(L, topIndex - 2);
    luaL_checknumber(L, topIndex - 1);
    luaL_checknumber(L, topIndex);
    glm::quat* euler = (glm::quat*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    *euler = glm::vec3(x, y, z);
    return 0;
}

int LuaTransfrom::SetEntityScale(lua_State* L)
{
    luaL_checknumber(L, 1);
    luaL_checknumber(L, 2);
    luaL_checknumber(L, 3);
    glm::vec3* scale = (glm::vec3*)lua_touserdata(L, lua_upvalueindex(1));
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    *scale = glm::vec3(x, y, z);
    return 0;
}

int LuaTransfrom::Translate(lua_State* L)
{
    int topIndex = lua_gettop(L);
    luaL_checknumber(L, topIndex - 2);
    luaL_checknumber(L, topIndex - 1);
    luaL_checknumber(L, topIndex);

    glm::vec3* position = (glm::vec3*)lua_touserdata(L, lua_upvalueindex(1));

    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);

    *position += glm::vec3(x, y, z);

    return 0;
}

int LuaTransfrom::GetEntityPosition(lua_State* L)
{
    glm::vec3* position = (glm::vec3*)lua_touserdata(L, lua_upvalueindex(1));
    lua_newtable(L);
    lua_pushstring(L, "x");
    lua_pushnumber(L, position->x);
    lua_settable(L, -3);
    lua_pushstring(L, "y");
    lua_pushnumber(L, position->y);
    lua_settable(L, -3);
    lua_pushstring(L, "z");
    lua_pushnumber(L, position->z);
    lua_settable(L, -3);
    return 1;
}

int LuaTransfrom::GetEntityRotation(lua_State* L)
{
    glm::vec3* euler = (glm::vec3*)lua_touserdata(L, lua_upvalueindex(1));
    lua_newtable(L);
    lua_pushstring(L, "x");
    lua_pushnumber(L, euler->x);
    lua_settable(L, -3);
    lua_pushstring(L, "y");
    lua_pushnumber(L, euler->y);
    lua_settable(L, -3);
    lua_pushstring(L, "z");
    lua_pushnumber(L, euler->z);
    lua_settable(L, -3);
    return 1;
}

int LuaTransfrom::GetEntityScale(lua_State* L)
{
    glm::vec3* scale = (glm::vec3*)lua_touserdata(L, lua_upvalueindex(1));
    assert(scale);
    lua_newtable(L);
    lua_pushstring(L, "x");
    lua_pushnumber(L, scale->x);
    lua_settable(L, -3);
    lua_pushstring(L, "y");
    lua_pushnumber(L, scale->y);
    lua_settable(L, -3);
    lua_pushstring(L, "z");
    lua_pushnumber(L, scale->z);
    lua_settable(L, -3);
    return 1;
}

const luaL_Reg LuaTransfrom::entityTransformLib[] =
{
    { NULL, NULL }
};
