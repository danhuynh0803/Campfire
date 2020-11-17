#pragma once

#include <lua.hpp>

class LuaScript : public ScriptableEntity
{
public:

    void Start();
    void Update(float);
    void Destroy();

private:
    lua_State* L;
};
