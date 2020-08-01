#include "ScriptEngine.h"

ScriptEngine::ScriptEngine()
{
	/*{
		auto NativePythagoras = [](lua_State* L) -> int
		{
			lua_Number a = lua_tonumber(L, -2);
			lua_Number b = lua_tonumber(L, -1);
			lua_Number csqr = (a * a) + (b * b);
			lua_pushnumber(L, csqr);
			return 1;
		};

		constexpr char* LUA_FILE = R"(
		function Pythagoras( a, b )
			csqr = NativePythagoras( a, b )
			return csqr, a, b
		end
		)";

		lua_State* L = luaL_newstate();
		lua_pushcfunction(L, NativePythagoras);
		lua_setglobal(L, "NativePythagoras");
		luaL_dostring(L, LUA_FILE);
		lua_getglobal(L, "Pythagoras");
		if (lua_isfunction(L, -1))
		{
			lua_pushnumber(L, 3);
			lua_pushnumber(L, 4);
			constexpr int NUM_ARGS = 2;
			constexpr int NUM_RETURNS = 3;
			lua_pcall(L, NUM_ARGS, NUM_RETURNS, 0);
			lua_Number c = lua_tonumber(L, -3);
			printf("csqr = %d\n", (int)c);
			lua_Number a = lua_tonumber(L, -2);
			printf("a = %d\n", (int)a);
			lua_Number b = lua_tonumber(L, -1);
			printf("b = %d\n", (int)b);
		}
		lua_close(L);
	}*/
	{
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);
		luaL_dofile(L,"../Assets/Scripts/test.lua");
		lua_getglobal(L, "a");
		if (lua_isnumber(L, -1))
		{
			int i = lua_tonumber(L, -1);
			printf("b = %d\n", (int)i);
		}
	}
}
ScriptEngine::~ScriptEngine()
{

}