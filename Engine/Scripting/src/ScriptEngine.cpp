#include "Scripting/ScriptEngine.h"
#include "Core/Log.h"

lua_State* ScriptEngine::L;

void ScriptEngine::RunFunction(const std::string& funcName, const std::string& filepath)
{
    luaL_dofile(L, filepath.c_str());
    //lua_pushcfunction(L, funcName.c_str());
    lua_setglobal(L, funcName.c_str());
    if (lua_isfunction(L, -1))
    {
        //lua_pcall(L, NUM_ARGS, NUM_RETURNS, 0);
    }
}

void ScriptEngine::Init()
{
    //the amount depends on the lua script
    //10 kB
    constexpr int POOL_SIZE = 1024 * 10;

    char memory[POOL_SIZE];
    ArenaAllocator pool(memory, &memory[POOL_SIZE - 1]);
    L = lua_newstate(ArenaAllocator::l_alloc, &pool);
    L ? Register() : printf("Error: Fail to initialize Lua stack\n");
}

void ScriptEngine::Register()
{
    if (!L) return;
}

void ScriptEngine::RunScript(const std::string& LuaScript)
{
    if (!L) return;
    int doResult = luaL_dostring(L, LuaScript.c_str());
    if (doResult != LUA_OK) printf("Error: %s\n", lua_tostring(L, -1));
}

void ScriptEngine::Shutdown()
{
    lua_close(L);
}