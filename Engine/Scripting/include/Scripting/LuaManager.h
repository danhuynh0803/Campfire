#pragma once
#include <functional>
#include <string>
#include <sstream>
#include "Events/LuaEvent.h"
#include "lua.hpp"
#include <nlohmann/json.hpp>

using LuaEventCallbackFn = std::function<void(LuaEvent&)>;
using JsonObject = nlohmann::json;

struct LuaData
{
    std::string sender;
    LuaEventCallbackFn EventCallback;
};

namespace LuaUtility
{
    bool is_number(const std::string& s);
    bool TransferTable(lua_State* L1, lua_State* L2);
    JsonObject SerializeLuaTable(lua_State*, JsonObject& json);
    int DeseralizeLuaTableX(lua_State* L);
    void DeseralizeLuaTable(lua_State*, const JsonObject&);
    void ParseLuaTableOnTop(lua_State*, const char*);
    int Log(lua_State* L);
}

class LuaManager
{
public:
    static void Init();
    static void Shutdown();
    static void SetEventCallback(const LuaEventCallbackFn&);
    static void SetGlobalLuaNumber(const char*, const lua_Number&);
    static void SetGlobalLuaInteger(const char*, const lua_Integer&);
    static void SetGlobalLuaString(const char*, const char*);
    static void SetGlobalLuaBoolean(const char*, const bool&);
    static bool SetGlobalLuaTable(lua_State* , const char*);
    static bool SetGlobalLuaTableNumber(const char*, const char*, const lua_Number&);
    static bool SetGlobalLuaTableInteger(const char*, const char*, const lua_Integer&);
    static bool SetGlobalLuaTableString(const char*, const char*, const char*);
    static bool SetGlobalLuaTableBoolean(const char*, const char*, const bool&);
    static bool SetGlobalLuaTableTable(lua_State*,const char* ,const char*);
    static bool GetGlobalLuaNumber(lua_State* , const char*);
    static bool GetGlobalLuaInteger(lua_State* , const char*);
    static bool GetGlobalLuaString(lua_State* , const char*);
    static bool GetGlobalLuaBoolean(lua_State* ,const char*);
    static bool GetGlobalLuaTable(lua_State*, const char*);

    void Find(const char* name);

private:
    static LuaData data;
    static lua_State* L;
};


class LuaEventDispatcher
{
public:
    LuaEventDispatcher(LuaEvent& e): event(e){}

    template<typename T, typename F>
    bool Dispatch(const F& func)
    {
        if (event.GetEventType() == T::GetStaticType())
        {
            event.handled = func(static_cast<T&>(event));
            return true;
        }
        return false;
    }

private:
    LuaEvent& event;
};
