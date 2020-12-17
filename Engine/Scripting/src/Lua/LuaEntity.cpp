#include "Scripting/LuaScript.h"
#include "Scripting/Lua/LuaEntity.h"
#include "Scripting/Lua/LuaTransform.h"
#include "Core/ResourceManager.h"
#include "Scene/Component.h"
#include "Tracy.hpp"

int LuaEntity::Instantiate(lua_State* L)
{
    int topIndex = lua_gettop(L);
    std::string prefabPath = luaL_checkstring(L, topIndex-3);
    lua_Number x = luaL_checknumber(L, topIndex-2);
    lua_Number y = luaL_checknumber(L, topIndex-1);
    lua_Number z = luaL_checknumber(L, topIndex);
    LuaScript* script = (LuaScript*)lua_touserdata(L, lua_upvalueindex(1));

    // Deserialize the requested prefab
    Entity newEntity = script->Instantiate(prefabPath, glm::vec3(x, y, z));
    if (!newEntity)
    {
        LOG_INFO("LuaEntity::Instantiate invalid Entity");
    }
    if (newEntity)
    {
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
    }
    
    return 1;
}

int LuaEntity::EntityDestroy(lua_State* L)
{
    int topIndex = lua_gettop(L);
    LuaScript* script = (LuaScript*)lua_touserdata(L, lua_upvalueindex(1));
    Entity entity = script->GetEntity();
    entity.AddComponent<Destroy>();
    lua_pushstring(L, "Destroy Entity");
    lua_error(L);
    return 0;
}

int LuaEntity::OtherEntityDestroy(lua_State* L)
{
    //LuaScript* script = (LuaScript*)lua_touserdata(L, lua_upvalueindex(1));
    Entity* entity = (Entity*)lua_touserdata(L, lua_upvalueindex(2));
    entity->AddComponent<Destroy>();
    return 0;
}

const luaL_Reg LuaEntity::entityLib[] =
{
    { NULL, NULL }
};
