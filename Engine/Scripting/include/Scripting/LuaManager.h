#pragma once
#include <functional>
#include <string>
#include <sstream>
#include "Events/LuaEvent.h"

class LuaManager
{    
public:
    using LuaEventCallbackFn = std::function<void(LuaEvent&)>;
    static void SetEventCallback(const LuaEventCallbackFn& callback) { data.EventCallback = callback; }
    
    struct LuaData
    {
        std::string title;

        LuaEventCallbackFn EventCallback;
    };

    static LuaData data;
};

class LuaEventDispatcher
{
public:
    LuaEventDispatcher(LuaEvent& e)
        : event(e)
    {
    }

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
