#pragma once

#include <string>

class Event;
//class LuaEvent;

struct Layer
{
    Layer(const std::string& _name = "Layer");
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate(float) {}
    virtual void OnImGuiRender() {}
    virtual void OnEvent(Event&) {}
    //virtual void OnLuaEvent(LuaEvent&) {}

    std::string name;
};
