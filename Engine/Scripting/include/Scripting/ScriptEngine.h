#pragma once

#include <lua.hpp>
#include "Scripting/ArenaAllocator.h"
#include <string>
#include <typeinfo>

class ScriptEngine
{
public:
    static void Init();
    static void RunFunction(const std::string& funcName, const std::string& scriptPath);
    static void RunScript(const std::string& LuaScript);
    static void Shutdown();

private:
    static lua_State* L;
    static void Register();
};
