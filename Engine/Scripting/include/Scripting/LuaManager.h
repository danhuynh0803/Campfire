#pragma once
#include <functional>
#include <string>
#include <sstream>
#include "lua.hpp"
#include "Events/LuaEvent.h"

//struct LuaData
//{
//    std::string sender;
//    LuaEventCallbackFn EventCallback;
//};

class LuaManager
{
public:
    static void Init();
    static void Shutdown();
    //static void SetEventCallback(const LuaEventCallbackFn&);
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
    static void Find(const char*);

    template <typename T>
    static void AddListener(std::function<void(T&)>);
    template<typename T>
    static void RemoveListener(std::function<void(T&)>);
    template <typename T>
    static void TriggerEvent(T&);

private:
    //static LuaData data;
    static lua_State* L;
    template <typename T>
    static std::vector<std::function<void(T&)>>& GetListeners();
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