#include "Scripting/LuaUtility.h"
#include <string>
#include <cstdlib>
#include <sstream>
#include "Core/Log.h"

#define LUA_DESTROY_ENTITY "Destroy Entity"

bool LuaUtility::is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool LuaUtility::TransferTable(lua_State* L1, lua_State* L2)
{
    if (!lua_istable(L1, -1)) return false;
    lua_pushnil(L1);
    while (lua_next(L1, -2) != 0)
    {
        if (lua_isinteger(L1, -2))
        {
            if (lua_isnil(L1, -1))
            {
                lua_pushinteger(L2, lua_tointeger(L1, -2));
                lua_pushnil(L2);
                lua_rawset(L2, -3);
            }
            else if (lua_isinteger(L1, -1))
            {
                lua_pushinteger(L2, lua_tointeger(L1, -2));
                lua_pushinteger(L2, lua_tointeger(L1, -1));
                lua_rawset(L2, -3);
            }
            else if (lua_isboolean(L1, -1))
            {
                lua_pushinteger(L2, lua_tonumber(L1, -2));
                lua_pushboolean(L2, lua_toboolean(L1, -1));
                lua_rawset(L2, -3);
            }
            else if (lua_isnumber(L1, -1))
            {
                lua_pushinteger(L2, lua_tointeger(L1, -2));
                lua_pushnumber(L2, lua_tonumber(L1, -1));
                lua_rawset(L2, -3);
            }
            else if (lua_isstring(L1, -1))
            {
                lua_pushinteger(L2, lua_tointeger(L1, -2));
                lua_pushstring(L2, lua_tostring(L1, -1));
                lua_rawset(L2, -3);
            }
            else if (lua_istable(L1, -1))
            {
                lua_pushinteger(L2, lua_tointeger(L1, -2));
                lua_newtable(L2);
                if (TransferTable(L1, L2))
                {
                    lua_settable(L2, -3);
                }
                else
                {
                    lua_pop(L2, 2);
                }
            }
        }
        else
        if (lua_isstring(L1, -2))
        {
            if (lua_isnil(L1, -1))
            {
                lua_pushstring(L2, lua_tostring(L1, -2));
                lua_pushnil(L2);
                lua_rawset(L2, -3);
            }
            else if (lua_isboolean(L1, -1))
            {
                lua_pushstring(L2, lua_tostring(L1, -2));
                lua_pushboolean(L2, lua_toboolean(L1, -1));
                lua_rawset(L2, -3);
            }
            else if (lua_isinteger(L1, -1))
            {
                lua_pushstring(L2, lua_tostring(L1, -2));
                lua_pushinteger(L2, lua_tointeger(L1, -1));
                lua_rawset(L2, -3);
            }
            else if (lua_isnumber(L1, -1))
            {
                lua_pushstring(L2, lua_tostring(L1, -2));
                lua_pushnumber(L2, lua_tonumber(L1, -1));
                lua_rawset(L2, -3);
            }
            else if (lua_isstring(L1, -1))
            {
                lua_pushstring(L2, lua_tostring(L1, -2));
                lua_pushstring(L2, lua_tostring(L1, -1));
                lua_rawset(L2, -3);
            }
            else if (lua_istable(L1, -1))
            {
                lua_pushstring(L2, lua_tostring(L1, -2));
                lua_newtable(L2);
                if (TransferTable(L1, L2))
                {
                    lua_settable(L2, -3);
                }
                else
                {
                    lua_pop(L2, 2);
                }
            }
        }
        lua_pop(L1, 1);
    }
    return true;
}

LuaUtility::JsonObject LuaUtility::SerializeLuaTable(lua_State* L, JsonObject& json)
{
    if (lua_istable(L, -1))
    {
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            if (lua_isinteger(L, -2))
            {
                if (lua_isnil(L, -1))
                {
                    json[std::to_string(lua_tointeger(L, -2))] = nullptr;
                }
                else if (lua_isboolean(L, -1))
                {
                    json[std::to_string(lua_tointeger(L, -2))] = (bool)lua_toboolean(L, -1);
                }
                else if (lua_isinteger(L, -1))
                {
                    json[std::to_string(lua_tointeger(L, -2))] = lua_tointeger(L, -1);
                }
                else if (lua_isnumber(L, -1))
                {
                    json[std::to_string(lua_tointeger(L, -2))] = lua_tonumber(L, -1);
                }
                else if (lua_isstring(L, -1))
                {
                    json[std::to_string(lua_tointeger(L, -2))] = std::string(lua_tostring(L, -1));
                }
                else if (lua_istable(L, -1))
                {
                    JsonObject subJson;
                    SerializeLuaTable(L, subJson);
                    if (!subJson.is_null())//empty table
                    {
                        json[std::to_string(lua_tointeger(L, -2))] = subJson;
                    }
                    else
                    {
                        json[std::to_string(lua_tointeger(L, -2))] = JsonObject({});
                    }
                }
            }
            else if (lua_isstring(L, -2))
            {
                if (lua_isnil(L, -1))
                {
                    json[std::string(lua_tostring(L, -2))] = nullptr;
                }
                else if (lua_isboolean(L, -1))
                {
                    json[std::string(lua_tostring(L, -2))] = (bool)lua_toboolean(L, -1);
                }
                else if (lua_isinteger(L, -1))
                {
                    json[std::string(lua_tostring(L, -2))] = lua_tointeger(L, -1);
                }
                else if (lua_isnumber(L, -1))
                {
                    json[std::string(lua_tostring(L, -2))] = lua_tonumber(L, -1);
                }
                else if (lua_isstring(L, -1))
                {
                    json[std::string(lua_tostring(L, -2))] = std::string(lua_tostring(L, -1));
                }
                else if (lua_istable(L, -1))
                {
                    JsonObject subJson;
                    SerializeLuaTable(L, subJson);
                    if (!subJson.is_null())
                    {
                        json[std::string(lua_tostring(L, -2))] = subJson;
                    }
                    else
                    {
                        //empty json {} = empty lua table {}!!
                        json[std::string(lua_tostring(L, -2))] = JsonObject({});
                    }
                }
            }
            lua_pop(L, 1);
        }
    }
    return json;
}

int LuaUtility::DeseralizeLuaTableX(lua_State* L)
{
    luaL_checktype(L, -1, LUA_TTABLE);
    JsonObject object;
    SerializeLuaTable(L, object);
    DeseralizeLuaTable(L, object);
    return 1;
}

void LuaUtility::DeseralizeLuaTable(lua_State* L, const JsonObject& jsonObject)
{
    //push a table back to the target lua state
    lua_newtable(L);
    for (auto& [key, value] : jsonObject.items())
    {
        //for integer key
        if (is_number(key))
        {
            if (value.is_null())
            {
                lua_pushinteger(L, std::atoi(key.c_str()));
                lua_pushnil(L);
                lua_rawset(L, -3);
            }
            else if (value.is_boolean())
            {
                lua_pushinteger(L, std::atoi(key.c_str()));
                lua_pushboolean(L, value);
                lua_rawset(L, -3);
            }
            else if (value.is_number_integer())
            {
                lua_pushinteger(L, std::atoi(key.c_str()));
                lua_pushinteger(L, value);
                lua_rawset(L, -3);
            }
            else if (value.is_number_float())
            {
                lua_pushinteger(L, std::atoi(key.c_str()));
                lua_pushnumber(L, value);
                lua_rawset(L, -3);
            }
            else if (value.is_string())
            {
                lua_pushinteger(L, std::atoi(key.c_str()));
                lua_pushstring(L, value.get<std::string>().c_str());
                lua_rawset(L, -3);
            }
            else if (value.is_array())
            {
                lua_pushinteger(L, std::atoi(key.c_str()));
                DeseralizeLuaTable(L, value);
                lua_rawset(L, -3);
            }
            else if (value.is_object())
            {
                lua_pushinteger(L, std::atoi(key.c_str()));
                DeseralizeLuaTable(L, value);
                lua_rawset(L, -3);
            }
        }
        else//string key
        {
            if (value.is_null())
            {
                lua_pushstring(L, key.c_str());
                lua_pushnil(L);
                lua_rawset(L, -3);
            }
            else if (value.is_boolean())
            {
                lua_pushstring(L, key.c_str());
                lua_pushboolean(L, value);
                lua_rawset(L, -3);
            }
            else if (value.is_number_integer())
            {
                lua_pushstring(L, key.c_str());
                lua_pushinteger(L, value);
                lua_rawset(L, -3);
            }
            else if (value.is_number_float())
            {
                lua_pushnumber(L, value);
                lua_pushstring(L, key.c_str());
                lua_rawset(L, -3);
            }
            else if (value.is_string())
            {
                lua_pushstring(L, key.c_str());
                lua_pushstring(L, value.get<std::string>().c_str());
                lua_rawset(L, -3);
            }
            else if (value.is_array())
            {
                lua_pushstring(L, key.c_str());
                DeseralizeLuaTable(L, value);
                lua_rawset(L, -3);
            }
            else if (value.is_object())
            {
                lua_pushstring(L, key.c_str());
                DeseralizeLuaTable(L, value);
                lua_rawset(L, -3);
            }
        }
    }
}

void LuaUtility::ParseLuaTableOnTop(lua_State* L, const char* tabs)
{
    if (lua_istable(L, -1))
    {
        std::stringstream tabStream;
        LOG_INFO("{0}{1}", tabs, "{");
        tabStream << tabs << "\t";
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            switch (lua_type(L, -1))//value type
            {
            case LUA_TNONE:
                CORE_INFO("What is the case for it to be NONE?");
                break;
            case LUA_TNIL:
                switch (lua_type(L, -2))//key type
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]: nil", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]: nil", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TBOOLEAN:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]: {3}", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2), lua_toboolean(L, -1));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]: {3}", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2), lua_toboolean(L, -1));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TLIGHTUSERDATA:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TNUMBER:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]: {3}", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2), lua_tonumber(L, -1));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]: {3}", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2), lua_tonumber(L, -1));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TSTRING:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]: \"{3}\"", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2), lua_tostring(L, -1));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]: \"{3}\"", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2), lua_tostring(L, -1));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TTABLE:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                    ParseLuaTableOnTop(L, tabStream.str().c_str());
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                    //LuaParseTableOnTop(tabStream.str().c_str());
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TFUNCTION:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TUSERDATA:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            case LUA_TTHREAD:
                switch (lua_type(L, -2))
                {
                case LUA_TNUMBER:
                    LOG_INFO("{0}({1})[{2}]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tointeger(L, -2));
                    break;
                case LUA_TSTRING:
                    LOG_INFO("{0}({1})[\"{2}\"]", tabStream.str().c_str(), lua_typename(L, lua_type(L, -1)), lua_tostring(L, -2));
                    break;
                default:
                    //other type of key
                    break;
                }
                break;
            default:
                CORE_INFO("What is the case for this? Something went wrong :(");
                break;
            }
            lua_pop(L, 1);
        }
        LOG_INFO("{0}{1}", tabs, "}");
    }
}

int LuaUtility::Log(lua_State* L)
{
    luaL_checkstring(L, -1);
    const char* msg = lua_tostring(L, -1);
    LOG_INFO(msg);
    return 0;
}

int LuaUtility::LuaCallback(lua_State* L)
{
    const char* message = lua_tostring(L, 1);
    if (message && strcmp(message, LUA_DESTROY_ENTITY) != 0)
    {
        //replace and push an error message that includes stack trace
        luaL_traceback(L, L, message, 1);
        return 1;
    }
    return 0;
}