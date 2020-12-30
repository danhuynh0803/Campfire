#pragma once
#include <string>
#include <functional>

namespace LuatGlobalEvent
{
    enum class Type
    {
        None = 0,
        LuaSetGlobalEvent
    };
}


//enum LuaEventCategory
//{
//    None = 0,
//    EventCategoryApplication = 1,
//    EventCategoryApplication2 = 2,
//    EventCategoryApplication3 = 4
//};

struct LuaEvent
{
    LuatGlobalEvent::Type type = LuatGlobalEvent::Type::None;
    LuaEvent(LuatGlobalEvent::Type type) : type(type) {}

    //bool handled = false;

    //virtual LuaEventType GetEventType() const = 0;
    //virtual const char* GetName() const = 0;
    ////virtual int GetCategoryFlags() const = 0;
    //virtual std::string ToString() const { return GetName(); }

    ////bool IsInCategory(LuaEventCategory category)
    ////{
    ////    return GetCategoryFlags() & category;
    ////}
};