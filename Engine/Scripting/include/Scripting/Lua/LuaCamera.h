#pragma once
#include <lua.hpp>

namespace LuaCamera
{
	int SetClearFlags(lua_State*);
	int GetClearFlags(lua_State*);
	int SetBackgroundColor(lua_State*);
	int GetBackgroundColor(lua_State*);
	int SetIsPerspective(lua_State*);
	int GetIsPerspective(lua_State*);
	int SetVerticalFOV(lua_State*);
	int GetVerticalFOV(lua_State*);
	int SetNearPlane(lua_State*);
	int GetNearPlane(lua_State*);
	int SetFarPlane(lua_State*);
	int GetFarPlane(lua_State*);
	int SetDepth(lua_State*);
	int GetDepth(lua_State*);
	int SetViewPortRectX(lua_State*);
	int GetViewPortRectX(lua_State*);
	int SetViewPortRectY(lua_State*);
	int GetViewPortRectY(lua_State*);
	int SetViewPortRectWidth(lua_State*);
	int GetViewPortRectWidth(lua_State*);
	int SetViewPortRectHeight(lua_State*);
	int GetViewPortRectHeight(lua_State*);
}