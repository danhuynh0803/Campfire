#include "Scripting/LuaManager.h"

LuaData LuaManager::data;

void LuaManager::SetEventCallback(const LuaEventCallbackFn& callback)
{
	data.EventCallback = callback;
}
