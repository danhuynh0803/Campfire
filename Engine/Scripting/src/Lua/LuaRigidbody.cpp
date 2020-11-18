#include "Scripting/Lua/LuaRigidbody.h"
#include "Physics/Rigidbody.h"

int LuaRigidbody::SetVelocity(lua_State* L)
{
	Rigidbody* rigidbody = (Rigidbody*)lua_getuservalue(L, lua_upvalueindex(1));
	lua_Number x = lua_tonumber(L, -3);
	lua_Number y = lua_tonumber(L, -2);
	lua_Number z = lua_tonumber(L, -1);
	rigidbody->SetVelocity(glm::vec3(x,y,z));
	return 0;
}
int LuaRigidbody::AddVelocity(lua_State* L)
{
	Rigidbody* rigidbody = (Rigidbody*)lua_getuservalue(L, lua_upvalueindex(1));
	lua_Number x = lua_tonumber(L, -3);
	lua_Number y = lua_tonumber(L, -2);
	lua_Number z = lua_tonumber(L, -1);
	rigidbody->SetVelocity(glm::vec3(x, y, z));
	return 0;
}

int LuaRigidbody::GetVelocity(lua_State* L)
{
	btVector3* velocity = (btVector3*)lua_getuservalue(L, lua_upvalueindex(1));
	lua_pushstring(L, "x");
	lua_pushnumber(L, velocity->getX());
	lua_settable(L, -3);
	lua_pushstring(L, "y");
	lua_pushnumber(L, velocity->getY());
	lua_settable(L, -3);
	lua_pushstring(L, "z");
	lua_pushnumber(L, velocity->getZ());
	lua_settable(L, -3);
	return 0;
}