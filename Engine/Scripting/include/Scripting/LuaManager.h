#pragma once
#include <functional>
#include <string>
#include <sstream>
#include "lua.hpp"
#include "Events/LuaEvent.h"
#include "nlohmann/json.hpp"

//struct LuaData
//{
//    std::string sender;
//    LuaEventCallbackFn EventCallback;
//};

struct LuaManager
{
    using JsonObject = nlohmann::json;
    static void Init();
    static void Shutdown();
    //static void SetEventCallback(const LuaEventCallbackFn&);
    static void SetGlobalLuaNumber(const char* name, const lua_Number& number);
    static void SetGlobalLuaInteger(const char* name, const lua_Integer& integer);
    static void SetGlobalLuaString(const char* name, const char* value);
    static void SetGlobalLuaBoolean(const char* name, const bool& value);
    static bool SetGlobalLuaTable(lua_State* L, const char* name);
    static bool SetGlobalLuaTableNumber(const char* table, const char* name, const lua_Number& number);
    static bool SetGlobalLuaTableInteger(const char* table, const char* name, const lua_Integer& integer);
    static bool SetGlobalLuaTableString(const char* table, const char* name, const char* value);
    static bool SetGlobalLuaTableBoolean(const char* table, const char* name, const bool& value);
    static bool SetGlobalLuaTableTable(lua_State* L, const char* table, const char* name);
    static bool GetGlobalLuaNumber(lua_State* L, const char* name);
    static bool GetGlobalLuaInteger(lua_State* L, const char* name);
    static bool GetGlobalLuaString(lua_State* L, const char* name);
    static bool GetGlobalLuaBoolean(lua_State* L, const char* name);
    static bool GetGlobalLuaTable(lua_State* L, const char* name);
    static JsonObject SerializeLuaTable(const char* name);
    static void Find(const char* name);

private:
    //static LuaData data;
    static lua_State* L;
};

//class LuaEventDispatcher
//{
//public:
//    LuaEventDispatcher(LuaEvent& e): event(e){}
//
//    template<typename T, typename F>
//    bool Dispatch(const F& func)
//    {
//        if (event.GetEventType() == T::GetStaticType())
//        {
//            event.handled = func(static_cast<T&>(event));
//            return true;
//        }
//        return false;
//    }
//
//private:
//    LuaEvent& event;
//};