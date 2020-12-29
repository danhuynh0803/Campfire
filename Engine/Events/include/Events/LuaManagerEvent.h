#pragma once
#include "Events/LuaEvent.h"

class LuaSetGlobalEvent : public LuaEvent
{
public:
    //static LuaEventType GetStaticType() { return LuaEventType::Global; }
    //LuaEventType GetEventType() const override { return GetStaticType(); }
    //const char* GetName() const override { return "None"; }
    ////int GetCategoryFlags() const override { return LuaEventCategory::EventCategoryApplication; }
private:
    std::string x;
};