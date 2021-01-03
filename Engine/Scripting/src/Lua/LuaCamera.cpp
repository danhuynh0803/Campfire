#include "Scripting/Lua/LuaCamera.h"
#include "Scene/Camera.h"

int LuaCamera::SetClearFlags(lua_State* L)
{
	int topIndex = lua_gettop(L);
	const char* type = luaL_checkstring(L, topIndex);
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/ClearFlag info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		camera->SetClearFlag(std::string(lua_tostring(L,-1)));
	}
	return 0;
}

int LuaCamera::GetClearFlags(lua_State* L)
{
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/ClearFlag info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		lua_pushstring(L, camera->GetClearFlag().c_str());
	}
	return 1;
}

int LuaCamera::SetBackgroundColor(lua_State* L)
{
	int topIndex = lua_gettop(L);
	lua_Number r = luaL_checknumber(L, topIndex - 3);
	lua_Number g = luaL_checknumber(L, topIndex - 2);
	lua_Number b = luaL_checknumber(L, topIndex - 1);
	lua_Number a = luaL_checknumber(L, topIndex);
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/BackgroundColor info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		camera->backgroundColor = glm::vec4(r, g, b, a);
	}
	return 0;
}

int LuaCamera::GetBackgroundColor(lua_State* L)
{
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/BackgroundColor info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		lua_newtable(L);
		lua_pushstring(L, "r");
		lua_pushnumber(L, camera->backgroundColor.r);
		lua_rawset(L, -3);
		lua_pushstring(L, "g");
		lua_pushnumber(L, camera->backgroundColor.g);
		lua_rawset(L, -3);
		lua_pushstring(L, "b");
		lua_pushnumber(L, camera->backgroundColor.b);
		lua_rawset(L, -3);
		lua_pushstring(L, "a");
		lua_pushnumber(L, camera->backgroundColor.a);
		lua_rawset(L, -3);
	}
	return 1;
}

int LuaCamera::SetIsPerspective(lua_State* L)
{
	int topIndex = lua_gettop(L);
	luaL_checktype(L, topIndex, LUA_TBOOLEAN);
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/IsPerspective info info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		camera->isPerspective = lua_toboolean(L, -1);
	}
	return 0;
}

int LuaCamera::GetIsPerspective(lua_State* L)
{
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/IsPerspective info info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		lua_pushboolean(L, camera->isPerspective);
	}
	return 1;
}

int LuaCamera::SetVerticalFOV(lua_State* L)
{
	int topIndex = lua_gettop(L);
	lua_Number verticalFOV = luaL_checknumber(L, topIndex);
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/VerticalFOV info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		camera->vFov = (float)verticalFOV;
	}
	return 0;
}

int LuaCamera::GetVerticalFOV(lua_State* L)
{
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/VerticalFOV info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		lua_pushnumber(L,camera->vFov);
	}
	return 1;
}

int LuaCamera::SetNearPlane(lua_State* L)
{
	int topIndex = lua_gettop(L);
	lua_Number nearPlane = luaL_checknumber(L, topIndex);
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/NearPlane info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		camera->nearPlane = (float)nearPlane;
	}
	return 0;
}

int LuaCamera::GetNearPlane(lua_State* L)
{
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/NearPlane info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		lua_pushnumber(L,camera->nearPlane);
	}
	return 1;
}

int LuaCamera::SetFarPlane(lua_State* L)
{
	int topIndex = lua_gettop(L);
	lua_Number farPlane = luaL_checknumber(L, topIndex);
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/FarPlane info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		camera->farPlane = (float)farPlane;
	}
	return 0;
}

int LuaCamera::GetFarPlane(lua_State* L)
{
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/farPlane info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		lua_pushnumber(L, camera->farPlane);
	}
	return 1;
}

int LuaCamera::SetDepth(lua_State* L)
{
	int topIndex = lua_gettop(L);
	lua_Number depth = luaL_checknumber(L, topIndex);
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/Depth info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		camera->depth = (float)depth;
	}
	return 0;
}

int LuaCamera::GetDepth(lua_State* L)
{
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/Depth info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		lua_pushnumber(L, camera->depth);
	}
	return 1;
}

int LuaCamera::SetViewPortRectX(lua_State* L)
{
	int topIndex = lua_gettop(L);
	lua_Number viewPortRectX = luaL_checknumber(L, topIndex);
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/ViewPortRectX info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		camera->x = (float)viewPortRectX;
	}
	return 0;
}

int LuaCamera::GetViewPortRectX(lua_State* L)
{
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/ViewPortRectX info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		lua_pushnumber(L, camera->x);
	}
	return 1;
}

int LuaCamera::SetViewPortRectY(lua_State* L)
{
	int topIndex = lua_gettop(L);
	lua_Number viewPortRectY = luaL_checknumber(L, topIndex);
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/ViewPortRectY info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		camera->y = (float)viewPortRectY;
	}
	return 0;
}

int LuaCamera::GetViewPortRectY(lua_State* L)
{
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/ViewPortRectY info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		lua_pushnumber(L, camera->y);
	}
	return 1;
}

int LuaCamera::SetViewPortRectWidth(lua_State* L)
{
	int topIndex = lua_gettop(L);
	lua_Number viewPortRectWidth = luaL_checknumber(L, topIndex);
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/ViewPortRectWidth info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		camera->width = (float)viewPortRectWidth;
	}
	return 0;
}

int LuaCamera::GetViewPortRectWidth(lua_State* L)
{
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/ViewPortRectWidth info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		lua_pushnumber(L, camera->width);
	}
	return 1;
}

int LuaCamera::SetViewPortRectHeight(lua_State* L)
{
	int topIndex = lua_gettop(L);
	lua_Number viewPortRectHeight = luaL_checknumber(L, topIndex);
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/ViewPortRectHeight info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		camera->height = (float)viewPortRectHeight;
	}
	return 0;
}

int LuaCamera::GetViewPortRectHeight(lua_State* L)
{
	Camera* camera = (Camera*)lua_touserdata(L, lua_upvalueindex(1));
	if (!camera)
	{
		lua_pushstring(L, "Missing Camera/ViewPortRectHeight info. Possibly was destroyed?");
		lua_error(L);
	}
	else
	{
		lua_pushnumber(L, camera->height);
	}
	return 1;
}