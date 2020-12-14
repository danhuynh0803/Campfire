#pragma once
#include <functional>
#include <string>
#include <sstream>

enum class LuaEventType
{
    None = 0,
    Test
};

enum LuaEventCategory
{
    None = 0,
    EventCategoryApplication = 1,
    EventCategoryApplication2 = 2,
    EventCategoryApplication3 = 4
};

class LuaEvent
{
public:
    bool handled = false;

    virtual LuaEventType GetEventType() const = 0;
    virtual const char* GetName() const = 0;
    virtual int GetCategoryFlags() const = 0;
    virtual std::string ToString() const { return GetName(); }

    //bool IsInCategory(LuaEventCategory category)
    //{
    //    return GetCategoryFlags() & category;
    //}
};

class Event2 : public LuaEvent
{
public:
    static LuaEventType GetStaticType() { return LuaEventType::None; }
    LuaEventType GetEventType() const override { return GetStaticType(); }
    const char* GetName() const override { return "None"; }
    int GetCategoryFlags() const override { return LuaEventCategory::EventCategoryApplication; }
private:
    std::string x;
};

using LuaEventCallbackFn = std::function<void(LuaEvent&)>;

struct LuaData
{
    std::string title;

    LuaEventCallbackFn EventCallback;
};

class X
{    
    void SetEventCallback(const LuaEventCallbackFn& callback) { data.EventCallback = callback; }
    LuaData data;
    //x->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
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
            //event.handled = func(static_cast<T&>(event));
            return true;
        }
        return false;
    }

private:
    LuaEvent& event;
};

void OnEventEvent(Event2& e)
{

}

void OnEvent(LuaEvent& e)
{
    OnEventEvent(static_cast<Event2&>(e));
    //LuaEventDispatcher dispatcher(e);
    //dispatcher.Dispatch<Event2>(std::bind(&OnEventEvent, nullptr, std::placeholders::_1));
}

