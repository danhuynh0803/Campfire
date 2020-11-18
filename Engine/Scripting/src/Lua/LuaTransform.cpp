#include "Scripting/Lua/LuaTransform.h"
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

int LuaTransfrom::NewTransform(lua_State* L)
{
    const char* transformCompoentMetaTableName = lua_tostring(L, lua_upvalueindex(1));
    void* transformComponent = lua_newuserdata(L, sizeof(TransformComponent));
    new (transformComponent) TransformComponent();//using the pointer to use the allocated memory that was already alloacted by lua
    luaL_getmetatable(L, transformCompoentMetaTableName);
    assert(lua_istable(L, -1));
    lua_setmetatable(L, -2);

    /*lua_newtable(L);
    lua_setuservalue(L, 1);*/

    return 1;
}

int LuaTransfrom::GetPosition(lua_State* L)
{
    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -1);
    lua_pushnumber(L, transfromComponet->position.x);
    lua_pushnumber(L, transfromComponet->position.y);
    lua_pushnumber(L, transfromComponet->position.z);

    return 1;
}

int LuaTransfrom::GetRotation(lua_State* L)
{
    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -1);
    lua_pushnumber(L, transfromComponet->rotation.x);
    lua_pushnumber(L, transfromComponet->rotation.y);
    lua_pushnumber(L, transfromComponet->rotation.z);
    return 1;
}

int LuaTransfrom::GetScale(lua_State* L)
{
    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -1);
    lua_pushnumber(L, transfromComponet->scale.x);
    lua_pushnumber(L, transfromComponet->scale.y);
    lua_pushnumber(L, transfromComponet->scale.z);
    return 1;
}

int LuaTransfrom::SetPosition(lua_State* L)
{
    assert(lua_isuserdata(L, -4));
    assert(lua_isnumber(L, -3));
    assert(lua_isnumber(L, -2));
    assert(lua_isnumber(L, -1));
    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -4);
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    transfromComponet->position = glm::vec3(x, y, z);
    return 0;
}

int LuaTransfrom::SetRotation(lua_State* L)
{
    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -4);
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    transfromComponet->euler = glm::vec3(x, y, z);
    return 0;
}

int LuaTransfrom::SetScale(lua_State* L)
{
    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -4);
    lua_Number x = lua_tonumber(L, -3);
    lua_Number y = lua_tonumber(L, -2);
    lua_Number z = lua_tonumber(L, -1);
    transfromComponet->scale = glm::vec3(x, y, z);
    return 0;
}

//int LuaTransfrom::ResetTransform(lua_State* L)
//{
//    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -1);
//    //call tc destructor
//    return 0;
//}

int LuaTransfrom::LuaTransformTableIndex(lua_State* L)
{
    assert(lua_isuserdata(L, -2));
    assert(lua_isstring(L, -1));

    TransformComponent* transfromComponet = (TransformComponent*)lua_touserdata(L, -2);
    const char* index = lua_tostring(L, -1);

    if (strcmp(index, "x") == 0)
    {
        lua_pushnumber(L, transfromComponet->position.x);
        return 1;
    }
    else if (strcmp(index, "y") == 0)
    {
        lua_pushnumber(L, transfromComponet->position.y);
        return 1;
    }
    else if (strcmp(index, "z") == 0)
    {
        lua_pushnumber(L, transfromComponet->position.z);
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

//TODO: __newIndex Mehod

const luaL_Reg LuaTransfrom::transformLib[] =
{
    { "SetPosition", LuaTransfrom::SetPosition },
    { "SetEuler", LuaTransfrom::SetRotation },
    { "SetScale", LuaTransfrom::SetScale },

    { "GetPosition", LuaTransfrom::GetPosition },
    { "GetEuler", LuaTransfrom::GetRotation },
    { "GetScale", LuaTransfrom::GetScale },
    { NULL, NULL }
};
