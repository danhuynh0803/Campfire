#pragma once
#include <lua.hpp>

namespace LuaCollider {
	namespace Box {
		int UpdateShape(lua_State*);
		int ShowData(lua_State*);
		int Reset(lua_State*);
		int SetIsTrigger(lua_State*);
		int GetIsTrigger(lua_State*);
		int SetCenter(lua_State*);
		int GetCenter(lua_State*);
	}
	namespace Sphere {
		int UpdateShape(lua_State*);
		int ShowData(lua_State*);
		int Reset(lua_State*);
		int SetIsTrigger(lua_State*);
		int GetIsTrigger(lua_State*);
		int SetCenter(lua_State*);
		int GetCenter(lua_State*);
	}
	namespace Capsule {
		int UpdateShape(lua_State*);
		int ShowData(lua_State*);
		int Reset(lua_State*);
		int SetIsTrigger(lua_State*);
		int GetIsTrigger(lua_State*);
		int SetCenter(lua_State*);
		int GetCenter(lua_State*);
	}
}