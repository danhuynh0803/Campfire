#pragma once

#include <lua.hpp>
#include "Scripting/ArenaAllocator.h"
#include <string>

class ScriptEngine
{
public:
    static void RunFunction(const std::string& funcName, const std::string& scriptPath);
    static void Init();
    static void Shutdown();

private:
    static lua_State* L;
};
