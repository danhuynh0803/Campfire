#include "Scripting/Lua/LuaRigidbody.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"

int LuaRigidbody::SetVelocity(lua_State* L)
{
	Entity* entity = (Entity*)lua_getuservalue(L, lua_upvalueindex(1));
	lua_Number x = lua_tonumber(L, -3);
	lua_Number y = lua_tonumber(L, -2);
	lua_Number z = lua_tonumber(L, -1);
	if (entity->HasComponent<RigidbodyComponent>())
	{
		entity->GetComponent<RigidbodyComponent>().rigidbody->SetVelocity(glm::vec3(x, y, z));
	}
	else
	{
		LOG_ERROR("Entity missing RigidbodyComponent")
	}

	return 0;
}
int LuaRigidbody::AddVelocity(lua_State* L)
{
	Entity* entity = (Entity*)lua_getuservalue(L, lua_upvalueindex(1));
	lua_Number x = lua_tonumber(L, -3);
	lua_Number y = lua_tonumber(L, -2);
	lua_Number z = lua_tonumber(L, -1);
	if (entity->HasComponent<RigidbodyComponent>())
	{
		entity->GetComponent<RigidbodyComponent>().rigidbody->AddVelocity(glm::vec3(x, y, z));
	}
	else
	{
		LOG_ERROR("Entity missing RigidbodyComponent")
	}
	return 0;
}

int LuaRigidbody::GetVelocity(lua_State* L)
{
	Entity* entity = (Entity*)lua_getuservalue(L, lua_upvalueindex(1));
	if (entity->HasComponent<RigidbodyComponent>())
	{
		lua_pushstring(L, "x");
		lua_pushnumber(L, entity->GetComponent<RigidbodyComponent>().rigidbody->GetVelocity().getX());
		lua_settable(L, -3);
		lua_pushstring(L, "y");
		lua_pushnumber(L, entity->GetComponent<RigidbodyComponent>().rigidbody->GetVelocity().getX());
		lua_settable(L, -3);
		lua_pushstring(L, "z");
		lua_pushnumber(L, entity->GetComponent<RigidbodyComponent>().rigidbody->GetVelocity().getX());
		lua_settable(L, -3);
	}
	else
	{
		LOG_ERROR("Entity missing RigidbodyComponent")
	}

	return 1;
}