#include "ScriptEngine.h"

void ScriptEngine::Init()
{
	LuaScripting::Init();
}
void ScriptEngine::Shutdown()
{
	LuaScripting::Shutdown();
}