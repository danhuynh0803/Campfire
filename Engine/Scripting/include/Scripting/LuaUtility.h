#pragma once
#include "lua.hpp"
#include <nlohmann/json.hpp>

namespace LuaUtility
{
    using JsonObject = nlohmann::json;

    bool is_number(const std::string& s);
    bool TransferTable(lua_State* L1, lua_State* L2);
    JsonObject SerializeLuaTable(lua_State*, JsonObject& json);
    int DeseralizeLuaTableX(lua_State* L);
    void DeseralizeLuaTable(lua_State* L, const JsonObject& json);
    void ParseLuaTableOnTop(lua_State* L, const char* tabs);
    int Log(lua_State* L);
    int LuaCallback(lua_State* L);
}