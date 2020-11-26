#pragma once

#include <lua.hpp>
#include "Scene/ScriptableEntity.h"

namespace LuaScriptCallBack {
    int lua_callback(lua_State*);
}

class LuaScript : public ScriptableEntity
{
public:

    void Start();
    void Update(float);
    void Destroy();

private:
    lua_State* L;
    void lua_pushcfunction_with_entity(const lua_CFunction& f, const char* name);
    void lua_pushcfunction_with_rigidbody(const lua_CFunction& f, const char* name);
    void lua_pushcfunction_with_tag(const lua_CFunction& f, const char* name);
    void lua_pushcfunction_with_audioSource(const lua_CFunction& f, const char* name);
    
    template<typename T>
    void lua_push_componet_table();
};
