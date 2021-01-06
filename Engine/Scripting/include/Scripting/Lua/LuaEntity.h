#pragma once
#include <lua.hpp>
#include <entt.hpp>

namespace LuaEntity
{
    void LuaPushEntity(lua_State* ,Entity);
    void LuaPushCFunctionWithAudioSource(lua_State* L, Entity entity, const lua_CFunction& f, const char* name);
    void LuaPushCFunctionWithCamera(lua_State*, Entity entity, const lua_CFunction& f, const char* name);
    void LuaPushCFunctionWithRigidbody(lua_State*, Entity entity, const lua_CFunction& f, const char* name);
    int Instantiate(lua_State*);
    int EntityDestroy(lua_State*);
    int OtherEntityDestroy(lua_State*);
    int AddComponent(lua_State*);
    int RemoveComponent(lua_State*);
    extern const luaL_Reg entityLib[];
}
