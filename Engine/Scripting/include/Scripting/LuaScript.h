#pragma once

#include <lua.hpp>
#include "Scene/ScriptableEntity.h"

namespace LuaScriptCallBack {
    int LuaCallback(lua_State*);
}

class LuaScript : public ScriptableEntity
{
public:
    void Start();
    void Update(float);
    void Destroy();
    void OnTriggerEnter(Entity);

private:
    lua_State* L;
    void LuaPushEntity(Entity entity);
    void LuaPushCFunctionWithEntity(const lua_CFunction& f, const char* name);
    void LuaPushCFunctionWithRigidbody(Entity entity, const lua_CFunction& f, const char* name);
    void LuaPushCFunctionWithRigidbody(const lua_CFunction& f, const char* name);
    void LuaPushCFunctionWithTag(const lua_CFunction& f, const char* name);
    void LuaPushCFunctionWithAudioSource(const lua_CFunction& f, const char* name);

    template<typename T>
    void LuaPushComponetTable();
};
