#pragma once
#include <lua.hpp>
#include "Scene/ScriptableEntity.h"

namespace LuaScriptCallBack {
    int LuaCallback(lua_State*);
}

class LuaScript : public ScriptableEntity
{

public:
    ~LuaScript()
    {
        lua_close(L);
    }

    void LoadStandardLibries();
    void Start();
    void Update(float deltaTime);
    void Destroy();

    void OnTriggerEnter(Entity entity);
    void OnTriggerStay(Entity entity);
    void OnTriggerExit(Entity entity);

private:
    lua_State* L;
    
    void LuaPushVectorTables();
    void LuaPushCFunctionWithAudioSource(const lua_CFunction& f, const char* name);
    void LuaPushCFunctionWithCamera(const lua_CFunction& f, const char* name);
    void LuaPushCFunctionWithEntity(const lua_CFunction& f, const char* name);
    void LuaPushCFunctionWithRigidbody(const lua_CFunction& f, const char* name);
    void LuaPushCFunctionWithTag(const lua_CFunction& f, const char* name);

    template<typename T>
    void LuaPushComponetTable();
};
