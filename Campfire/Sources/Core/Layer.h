#ifndef LAYER_H
#define LAYER_H

#include <string>

struct Layer
{
    Layer(const std::string& _name = "Layer");
    virtual ~Layer() = default;

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void OnUpdate() {}
    virtual void OnImGuiRender() {}
    virtual void OnEvent() {}

    std::string name;
};

#endif // LAYER_H
