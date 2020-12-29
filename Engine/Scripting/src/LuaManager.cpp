#include "Scripting/LuaManager.h"
#include "Scripting/LuaUtility.h"

//LuaData LuaManager::data;
lua_State* LuaManager::L;

void LuaManager::Init()
{
    L = luaL_newstate();
    luaL_openlibs(L);
}

void LuaManager::Shutdown()
{
    lua_close(L);
}

//void LuaManager::SetEventCallback(const LuaEventCallbackFn& callback)
//{
//	data.EventCallback = callback;
//}

void LuaManager::SetGlobalLuaNumber(const char* name, const lua_Number& number)
{
    lua_pushnumber(L, number);
    lua_setglobal(L, name);
}

void LuaManager::SetGlobalLuaInteger(const char* name, const lua_Integer& integer)
{
    lua_pushinteger(L, integer);
    lua_setglobal(L, name);
}

void LuaManager::SetGlobalLuaString(const char* name, const char* string)
{
    lua_pushstring(L, string);
    lua_setglobal(L, name);
}

void LuaManager::SetGlobalLuaBoolean(const char* name, const bool& boolean)
{
    lua_pushboolean(L, boolean);
    lua_setglobal(L, name);
}

bool LuaManager::SetGlobalLuaTable(lua_State* from, const char* name)
{
    //check name here to prevent bad assignment
    if (!lua_istable(from, -1)) return false;
    lua_newtable(L);
    if (!LuaUtility::TransferTable(from, L))
    {
        lua_pop(L, 1);
        return false;
    }
    lua_setglobal(L, name);
    return true;
}

bool LuaManager::SetGlobalLuaTableNumber(const char* table, const char* name, const lua_Number& number)
{
    lua_getglobal(L, table);
    lua_pushnumber(L, number);
    lua_setfield(L, -2, name);
    lua_setglobal(L, table);
    return true;
}

bool LuaManager::SetGlobalLuaTableInteger(const char* table, const char* name, const lua_Integer& integer)
{
    //To be implemented
    //lua_setglobal(L, name);
    return true;
}

bool LuaManager::SetGlobalLuaTableString(const char* table, const char* name, const char* string)
{
    //To be implemented
    //lua_setglobal(L, name);
    return true;
}

bool LuaManager::SetGlobalLuaTableBoolean(const char* table, const char* name, const bool& boolean)
{
    //To be implemented
    //lua_setglobal(L, name);
    return true;
}

bool LuaManager::SetGlobalLuaTableTable(lua_State* from, const char* table, const char* name)
{
    return true;
}

bool LuaManager::GetGlobalLuaNumber(lua_State* to, const char* name)
{
    lua_getglobal(L, name);
    if(lua_isnumber(L,-1))
    {
        lua_pushnumber(to, lua_tonumber(L, -1));
        lua_pop(L, 1);
        return true;
    }
    lua_pop(L, 1);
    return false;
}

bool LuaManager::GetGlobalLuaInteger(lua_State* to, const char* name)
{
    lua_getglobal(L, name);
    if (lua_isinteger(L, -1))
    {
        lua_pushinteger(to, lua_tointeger(L, -1));
        lua_pop(L, 1);
        return true;
    }
    lua_pop(L, 1);
    return false;
}

bool LuaManager::GetGlobalLuaString(lua_State* to, const char* name)
{
    lua_getglobal(L, name);
    if (lua_isstring(L, -1))
    {
        lua_pushstring(to, lua_tostring(L, -1));
        lua_pop(L, -1);
        return true;
    }
    lua_pop(L, 1);
    return false;
}

bool LuaManager::GetGlobalLuaBoolean(lua_State* to ,const char* name)
{
    lua_getglobal(L, name);
    if (lua_isboolean(L, -1))
    {
        lua_pushboolean(to, lua_toboolean(L, -1));
        lua_pop(L, 1);
        return true;
    }
    lua_pop(L, 1);
    return false;
}

bool LuaManager::GetGlobalLuaTable(lua_State* to, const char* name)
{
    lua_getglobal(L, name);
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return false;
    }
    lua_newtable(to);
    if (!LuaUtility::TransferTable(L, to))
    {
        lua_pop(to,1);
        return false;
    }
    lua_pop(L, 1);//pop the very first table from the getglobal
    return true;
}

LuaManager::JsonObject LuaManager::SerializeLuaTable(const char* name)
{
    lua_getglobal(L, name);
    JsonObject json;
    LuaUtility::SerializeLuaTable(L, json);
    lua_pop(L, 1);
    return json;
}

void LuaManager::Find(const char* name)
{
    //might not be plausible
}

//JsonObject LuaUtility::SerializeLuaTable(lua_State* L, JsonObject& json)
//{
//    if (lua_istable(L, -1))
//    {
//        lua_pushnil(L);
//        while (lua_next(L, -2) != 0)
//        {
//            JsonObject subJson;
//            switch (lua_type(L, -1))//value type
//            {
//            case LUA_TNIL:
//                switch (lua_type(L, -2))//key type
//                {
//                case LUA_TNUMBER:
//                    json[std::to_string(lua_tointeger(L, -2))] = nullptr;
//                    break;
//                case LUA_TSTRING:
//                    json[std::string(lua_tostring(L, -2))] = nullptr;
//                    break;
//                default:
//                    //other type of key
//                    break;
//                }
//                break;
//            case LUA_TBOOLEAN:
//                switch (lua_type(L, -2))
//                {
//                case LUA_TNUMBER:
//                    json[std::to_string(lua_tointeger(L, -2))] = lua_toboolean(L, -1);
//                    break;
//                case LUA_TSTRING:
//                    json[std::string(lua_tostring(L, -2))] = lua_toboolean(L, -1);
//                    break;
//                default:
//                    //other type of key
//                    break;
//                }
//                break;
//            case LUA_TNUMBER:
//                switch (lua_type(L, -2))
//                {
//                case LUA_TNUMBER:
//                    if (lua_isinteger(L, -1))
//                    {
//                        json[std::to_string(lua_tointeger(L, -2))] = lua_tointeger(L, -1);
//                    }
//                    else
//                    {
//                        json[std::to_string(lua_tointeger(L, -2))] = lua_tonumber(L, -1);
//                    }
//                    break;
//                case LUA_TSTRING:
//                    json[std::string(lua_tostring(L, -2))] = lua_tonumber(L, -1);
//                    break;
//                default:
//                    break;
//                }
//                break;
//            case LUA_TSTRING:
//                switch (lua_type(L, -2))
//                {
//                case LUA_TNUMBER:
//                    json[std::to_string(lua_tointeger(L, -2))] = std::string(lua_tostring(L, -1));
//                    break;
//                case LUA_TSTRING:
//                    json[std::string(lua_tostring(L, -2))] = std::string(lua_tostring(L, -1));
//                    break;
//                default:
//                    //other type of key
//                    break;
//                }
//                break;
//            case LUA_TTABLE:
//                switch (lua_type(L, -2))
//                {
//                case LUA_TNUMBER:
//                    json[std::to_string(lua_tointeger(L, -2))] = SerializeLuaTable(L, subJson);
//                    break;
//                case LUA_TSTRING:
//                    json[std::string(lua_tostring(L, -2))] = SerializeLuaTable(L, subJson);
//                    break;
//                default:
//                    //other type of key
//                    break;
//                }
//                break;
//            default:
//                break;
//            }
//            lua_pop(L, 1);
//        }
//    }
//    return json;
//}