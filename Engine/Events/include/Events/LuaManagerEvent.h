#pragma once
#include "Events/LuaEvent.h"

struct LuaSetGlobalEvent : public LuaEvent
{
public:
    std::string x;
    LuaSetGlobalEvent(const std::string& x ) : LuaEvent(LuaEventType::LuaSetGlobalEvent) {}
    
    //static LuaEventType GetStaticType() { return LuaEventType::Global; }
    //LuaEventType GetEventType() const override { return GetStaticType(); }
    //const char* GetName() const override { return "None"; }
    ////int GetCategoryFlags() const override { return LuaEventCategory::EventCategoryApplication; }
};