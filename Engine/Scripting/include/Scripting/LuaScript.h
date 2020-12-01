#pragma once
#include <lua.hpp>
#include "Scene/ScriptableEntity.h"

class LuaScript : public ScriptableEntity
{
public:
    ~LuaScript()
    {
        lua_close(L);
    }

    void Start();
    void Update(float);
    void Destroy();

    void OnTriggerEnter(Entity);
    void OnTriggerStay(Entity);
    void OnTriggerExit(Entity);

    void LuaPushEntity(Entity, lua_State*);
private:
    lua_State* L;
    lua_State* StartCoroutine;
    lua_State* UpdateCoroutine;

    void LuaPushCFunctionWithEntity(lua_State*, const lua_CFunction&, const char*);
    void LuaPushCFunctionWithRigidbody(lua_State*, Entity, const lua_CFunction&, const char*);
    void LuaPushCFunctionWithRigidbody(lua_State*, const lua_CFunction&, const char*);
    void LuaPushCFunctionWithTag(lua_State*, const lua_CFunction&, const char*);
    void LuaPushCFunctionWithAudioSource(lua_State*, const lua_CFunction&, const char*);
    template<typename T>
    void LuaPushComponetTable();
};
