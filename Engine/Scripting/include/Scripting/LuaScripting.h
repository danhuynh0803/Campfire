#ifndef LUA_SCRIPTING_H
#define LUA_SCRIPTING_H

#include "lua.hpp"
#include "Scripting/ArenaAllocator.h"

class LuaScripting
{
public:
    static void Init();
    static void Shutdown();
private:
    static lua_State* L;
};

#endif // !LUA_SCRIPTING_H
