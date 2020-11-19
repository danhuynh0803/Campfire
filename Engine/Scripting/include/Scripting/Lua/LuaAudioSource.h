#pragma once

#include <lua.hpp>

namespace LuaAudioSource {
	int Play(lua_State*);
	int Pause(lua_State*);
	int Stop(lua_State*);
}
