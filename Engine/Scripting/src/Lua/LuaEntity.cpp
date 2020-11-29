#include "Scripting/LuaScript.h"
#include "Scripting/Lua/LuaEntity.h"
#include "Scripting/Lua/LuaTransform.h"
#include "Core/ResourceManager.h"

int LuaEntity::Instantiate(lua_State* L)
{
    luaL_checkstring(L, 1);//path to .prefab or something
    luaL_checknumber(L, 2);
    luaL_checknumber(L, 3);
    luaL_checknumber(L, 4);

    lua_Number x = lua_tonumber(L, 2);
    lua_Number y = lua_tonumber(L, 3);
    lua_Number z = lua_tonumber(L, 4);
    LuaScript* script = (LuaScript*)lua_touserdata(L, lua_upvalueindex(1));

    // Deserialize the requested prefab
    std::string prefabPath = std::string(lua_tostring(L, 1));
    Entity newEntity = script->Instantiate(prefabPath, glm::vec3(x, y, z));
    //Entity newEntity = script->Instantiate(other, glm::vec3(x,y,z));
    lua_newtable(L);
    {
        lua_newtable(L);
        {
            lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::SetEntityPosition, 1);
            lua_setfield(L, -2, "SetPosition");

            lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().euler));
            lua_pushcclosure(L, LuaTransfrom::SetEntityRotation, 1);
            lua_setfield(L, -2, "SetRotation");

            lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().scale));
            lua_pushcclosure(L, LuaTransfrom::SetEntityScale, 1);
            lua_setfield(L, -2, "SetScale");

            lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::Translate, 1);
            lua_setfield(L, -2, "Translate");

            lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().position));
            lua_pushcclosure(L, LuaTransfrom::GetEntityPosition, 1);
            lua_setfield(L, -2, "GetPosition");

            lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().rotation));
            lua_pushcclosure(L, LuaTransfrom::GetEntityRotation, 1);
            lua_setfield(L, -2, "GetRotation");

            lua_pushlightuserdata(L, &(newEntity.GetComponent<TransformComponent>().scale));
            lua_pushcclosure(L, LuaTransfrom::GetEntityScale, 1);
            lua_setfield(L, -2, "GetScale");
        }
        lua_setfield(L, -2, "Transform");
    }
    return 1;
}

const luaL_Reg LuaEntity::entityLib[] =
{
    { NULL, NULL }
};
