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
    std::string title;
    LuaEventCallbackFn EventCallback;
};

namespace LuaUtility
{
    void TransferTable(lua_State* L1, lua_State* L2);
    JsonObject SerializeLuaTable(lua_State*, JsonObject& json);
    void DeseralizeLuaTable(lua_State*, JsonObject);
    void ParseLuaTableOnTop(lua_State*, const char*);
}

class LuaManager
{
public:
    static void Init();
    static void Shutdown();
    static void SetEventCallback(const LuaEventCallbackFn& callback);
    static void SetGlobalLuaNumber(const char*, const lua_Number&);
    static void SetGlobalLuaInteger(const char*, const lua_Integer&);
    static void SetGlobalLuaString(const char*, const char*);
    static void SetGlobalLuaBoolean(const char*, const bool&);
    static void SetGlobalLuaTable(const char*, lua_State*);

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
