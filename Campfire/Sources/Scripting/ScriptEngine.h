#ifndef SCRIPT_ENGINE_H
#define SCRIPT_ENGINE_H

#include "LuaScripting.h"

class ScriptEngine
{
public:
	static void Init();
	static void Shutdown();
};

#endif // !SCRIPT_ENGINE_H